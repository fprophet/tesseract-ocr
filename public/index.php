<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tesseract OCR</title>
    <link rel="stylesheet" href="/resources/main.css?v=" <?= time()?>>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.2/css/all.min.css" integrity="sha512-SnH5WK+bZxgPHs44uWIX+LLJAJ9/2PkPKZ5QiAj6Ta86w+fsb2TkcmfRyVX3pBnMFcV7oQPJkl9QevSCWr3W6A==" crossorigin="anonymous" referrerpolicy="no-referrer" />
</head>
<body>
    <header>
        <div id="image-holder">
            <img width="120" src="/resources/images/logo-image.jpg" alt="">
            <span>Tesseract OCR</span>
        </div>
        <div id="nav-list"></div>
    </header>

    <section id="main">
        <div id="upload-section">
            <div id="input-section">
                <div class="file-input-wrapper">
                    <div class="dropzone" data-text="Select your file!">
                        <img src="http://100dayscss.com/codepen/upload.svg" class="upload-icon" />
                        <input type="file" id="upload" class=upload-input>
                    </div>
                    <div class="buttons-holder">
                        <label class="btn" for="upload" name="file-button" id="file-button" >Upload file</label>
                        <i class="options-toggle fa-solid fa-gear"></i>
                    </div>
                    <div class="options-wrapper">
                        <div class="input-group">
                            <label for="process-image">Pre Process Image</label>
                            <input type="checkbox" name="process-image" id="process-image">
                        </div>
                        <div class="input-group">
                            <label for="deskew">Deskew</label>
                            <input type="checkbox" name="deskew" id="deskew">
                        </div>
                        <div class="input-group">
                            <label for="process-image">Process Image</label>
                            <input type="checkbox" name="process-image" id="process-image">
                        </div>
                    </div>

                    <button class="start-process">Start Process</button>               
                </div>
            </div>

            <div id="image-preview">
                
                <div class="main-text">
                        <h1>Tesseract text recognition and extraction</h1>
                        <p>This is web interface for the C++ program that uses Tesseract API 
                            for OCR-ing a custom image.                            
                        </p>
                        <p>The purpose of this project is to show the OCR process step by step and experiment with different
                            paramenters to see which would obtain better results in terms of detection accuracy.
                        </p>
                        <p>
                            To use this program simply upload a image of choice from which you want to extract the text and click
                            "Start process". In real time the pre procesed image is displayed in it's current state.
                        </p>
                </div>
                <div class="preview-holder" >
                    <span class="preview-name"></span>
                    <img class="upload-preview" src="#" alt="">
                </div>
            </div>
          
        </div>
        <hr/>
        <div id="output-wrapper">
            <div id="output-text">

            </div>
        </div>
        <hr>
        <div id="processed-images-preview">
            <!-- <div class="processed-image-preview"></div> -->
            <!-- <div class="processed-image-preview"></div> -->

            <!-- <div class="processed-image-preview"></div> -->

        </div>
    </section>
    
    <footer></footer>
    <script src="/resources/main.js?v=" <?= time()?>></script>
</body>
</html>