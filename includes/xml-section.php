<?php

class Section {

    public $pgSzW = 0;
    public $pgSzH = 0;
    public $cols = 0;
    public $colsSpace = 0;
    public $cols_sep = 0;
    public $type = '';
    public $margins = [];
    public $orient = 'portrait';
    public $section_xml = '';
    public $section_text = '';

    public function __construct($style) {
        if (is_array($style) && !empty($style)) {
            foreach ($style as $key => $val) {
                switch ($key) {
                    case 'pgSzW':
                        $this->pgSzW = $val;
                        break;
                    case 'pgSzH':
                        $this->pgSzH = $val;
                        break;
                    case 'cols':
                        $this->cols = $val;
                        break;
                    case 'colsSpace':
                        $this->colsSpace = $val;
                        break;
                    case 'type':
                        $this->type = $val;
                        break;
                    case 'orient':
                        $this->orient = $val;
                        break;
                    case 'margins':
                        foreach ($val as $k => $v) {
                            switch ($k) {
                                case 'top':
                                    $this->margins['top'] = $v;
                                    break;
                                case 'bottom':
                                    $this->margins['bottom'] = $v;
                                    break;
                                case 'left':
                                    $this->margins['left'] = $v;
                                    break;
                                case 'right':
                                    $this->margins['right'] = $v;
                                    break;
                            }
                        }
                        break;
                }
            }
        }

        $this->createXml();
    }

    public function add_column_break() {
        $this->section_text .= '<w:p><w:r><w:br w:type="column"/></w:r></w:p>';
    }

    public function add_page_break() {
        $this->section_text .= '<w:p>
        <w:r>
          <w:br w:type="page" />
        </w:r>
      </w:p>';
    }

    public function createXml() {
        $this->section_xml = '<w:sectPr>';
        if ($this->pgSzW !== 0 || $this->pgSzH !== 0) {
            $this->section_xml .= '<w:pgSz w:w="' . $this->pgSzW . '" w:h="' . $this->pgSzH . '" w:orient="' . $this->orient . '"/>';
        }

        if (!empty($this->margins)) {
            $this->section_xml .= '<w:pgMar ';
            foreach ($this->margins as $key => $val) {
                $this->section_xml .= 'w:' . $key . '="' . $val . '" ';
            }
            $this->section_xml .= '/>';
        }

        if ($this->cols !== 0) {
            $this->section_xml .= '<w:cols w:num="' . $this->cols . '" w:space="' . $this->colsSpace . '" w:equalWidth="1">';
            // for( $i=0; $i < $this->cols; $i++){
            //     $this->section_xml  .= '<w:col />';
            // }
            $this->section_xml .= '</w:cols>';
        }
        if ($this->type == '') {
            $this->type = 'continuous';
        }
        $this->section_xml .= '<w:type w:val="' . $this->type . '" />';
        $this->section_xml .= '</w:sectPr>';

        // echo var_dump($this->section_xml );
        return $this->section_xml;
    }

    public function addPara(){
        $this->section_text .= '<w:p>';
    }
    public function endPara(){
        $this->section_text .= '</w:p>';
    }

    public function addLine(){
        $this->section_text .= '<w:r> <w:rPr><w:sz w:val="20"/>';
        $this->section_text .= '</w:rPr>';

    }

    public function endLine(){
        $this->section_text .= '</w:r><w:br/>';
    }

    public function addText($text, $font_size, $x, $y, $w, $h,$bold = false, $italic = false, $end_para = false) {

//                $this->section_text .= '<w:p><w:pPr><w:framePr w:w="' . $w . '" w:h="' . $h . '" w:hRule="auto" w:wrap="auto" w:vAnchor="page" w:hAnchor="page" w:x="' . $x . '" w:y="' .$y . '"/><w:spacing w:before="10" w:after="10" w:beforeAutospacing="0" w:afterAutospacing="0"/></w:pPr>' . $text . '</w:p>';
        //styling
        $this->section_text .= '';
        
        // if( $bold ){
        //     $this->section_text .= '<w:b/>';
        // }
        // if( $italic ){
        //     $this->section_text .= '<w:i/>';
        // }
        //end styling
        $this->section_text .= '<w:t xml:space="preserve">' . $text . '</w:t>';

        // if($end_para ){
        //     $this->section_text .= "</w:p>";
        // }
    }

    private function _pixel_to_twip($val) {

        $pixel_to_inch = ((float) ($val / 96));
        $inch_to_twip = $this->_inch_to_twip($pixel_to_inch);

        return $inch_to_twip;
    }

    private function _inch_to_twip($val) {
        return (float) $val * 1440;
    }

    private function _escape_text($text) {
        $text = str_replace("&", "&amp;", $text);
        $text = str_replace("'", "&apos;", $text);
        $text = str_replace('"', "&quot;", $text);
        $text = str_replace(">", "&gt;", $text);
        $text = str_replace("<", "&lt;", $text);
        return $text;
    }

    public function createTable($table, $wp, $hp) {

        $this->section_text .= '<w:tbl><w:tblPr><w:tblStyle w:val="TableGrid"/></w:tblPr>';
        $this->section_text .= '<w:tblGrid>'; //</w:tblGrid>';
        $total_cols = ($table["columns"]);
        $width = 0;
//            echo var_dump($table);
        for ($i = 0; $i < $total_cols; $i++) {
            $width = $table["column_widths"][$i];
            $this->section_text .= '<w:gridCol w:w="' . $this->_pixel_to_twip($width) * $wp . '"/>';
        }
        $this->section_text .= '</w:tblGrid>';
        $row_count = 0;
        $rows = [];
        for ($i = 1; $i <= $table["rows"]; $i++) {
            $rows[$i] = array_map(function ($cell) use ($i) {
                $row_positions = [];
                if (strpos($cell["info"]["row"], "+") !== false) {
                    $row_positions = explode("+", $cell["info"]["row"]);
                }
//                echo var_dump($row_positions);
                if ($cell["info"]["row"] == strval($i)) {
//                     echo var_dump($cell);
                    return $cell;
                } elseif (in_array(($i), $row_positions) !== false) {
                    return $cell;
                }
            }, $table["cells"]);
            $rows[$i] = array_filter($rows[$i], function ($item) {
                if (null !== $item) {
                    return $item;
                }
            });
            $rows[$i] = array_values($rows[$i]);

            usort($rows[$i], "x_sort");
//        echo var_dump($rows[$i]) . "\n -- ----------------- \n";
        }
//        echo var_dump($rows);

        foreach ($rows as $ind => $row) {
            if (count($row) == 0) {
                continue;
            }
            $heights = array_map(function ($cell) {
                if (strpos($cell["info"]["row"], "+") == false) {
                    return $cell["info"]["height"];
                }
            }, $row);
            $max_height = max($heights);
            $this->section_text .= '<w:tr>';
            $this->section_text .= '<w:trPr><w:trHeight w:val="' . $this->_pixel_to_twip($max_height) * $wp . '" w:hRule="exact"/><w:jc w:val="end"/></w:trPr>';
//            echo var_dump(count($row));
            for ($i = 0; $i < count($row); $i++) {
                $this->section_text .= '<w:tc><w:tcPr>';
                $col_width = $row[$i]["info"]["width"];
                $span = 0;
                // echo var_dump($table["column_widths"][$i]);
                //echo var_dump($col_width);
//                echo "-------------\n";
                if (($col_width / $table["column_widths"][$i]) >= 2) {
                    $span = $col_width / $table["column_widths"][$i];
//                    echo var_dump($col_width);
//                    echo var_dump($table["column_widths"][$i]);
//                    echo var_dump($ind);
//                                        echo var_dump($i);
//
//                                    echo " \n ------------ \n";
                }

                $xml_span = '';
//                echo var_dump($table["column_widths"][$i]);
//                echo var_dump($row[$i]["info"]["width"]);
//                echo var_dump($ind);
//                echo " \n ------------ \n";
                $current_total_width = $table["column_widths"][$i];
                //CALCULATE CELL SPAN:
                $total_span = 1;
                $k = $i + 1;
                $flag = false;
                while ($current_total_width < $col_width && $k < count($table["column_widths"])) {
                    $current_total_width += $table["column_widths"][$k];
                    if ($current_total_width < $col_width) {
                        $total_span++;
                    }
                    $k++;
                }
                if ($total_span >= 2) {
                    $xml_span = '<w:gridSpan w:val="' . $total_span . '"/>';
                } elseif ($total_span == 0) {
                    $xml_span = '<w:gridSpan w:val="' . 1 . '"/>';
                }
                $merge = '';
                if (strpos($row[$i]["info"]["row"], "+") !== false) {
                    $row_positions = explode("+", $row[$i]["info"]["row"]);
                    if (in_array($ind, $row_positions)) {
                        $merge .= '<w:vMerge';
                        if ($ind == $row_positions[0]) {
                            $merge .= ' w:val="restart"/>';
                        } else {
                            $merge .= '/>';
                        }
                    }
                }
                //echo var_dump($span);
//                echo var_dump($span);
//                                echo var_dump(    $col_width);

                $combined_text = "";
                foreach ($row[$i]["text"] as $text) {
                    $combined_text .= $text . "\n";
                }
//                    echo var_dump($combined_text);
//                    echo var_dump($row[$i]["text_info"]["conf"]);
                //
                $this->section_text .= '<w:tcBorders>
<w:top w:val="single" w:sz="5" w:space="0" w:color="FF0000"></w:top>
<w:start w:val="single" w:sz="5" w:space="0" w:color="FF0000"></w:start>
<w:bottom w:val="single" w:sz="5" w:space="0" w:color="FF0000"></w:bottom>
<w:end w:val="single" w:sz="5" w:space="0" w:color="FF0000"></w:end>
</w:tcBorders>' . $xml_span . '<w:tcW w:w="' . $this->_pixel_to_twip($current_total_width) * $wp . '" w:type="dxa" />' . $merge . '</w:tcPr><w:p><w:r><w:rPr><w:sz w:val="' . $row[$i]["text_info"]["size"][0] . '"/></w:rPr><w:t>' . $this->_escape_text($combined_text) . '</w:t></w:r></w:p></w:tc>';
            }
            $this->section_text .= '</w:tr>';
            $row_count++;
        }
        $this->section_text .= '</w:tbl>';
    }

}

?>