<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tesseract OCR</title>
    <link rel="stylesheet" href="/resources/main.css?v=<?= time()?>">
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
                    <div class="file-upload-wrapper" data-text="Select your file!">
                        <input name="file-upload-field" type="file" class="file-upload-field" value="">
                    </div>

                    <div class="buttons-wrapper">
                        <div class="language-wrapper">
                            <label for="language">Language</label>
                            <select name="language" id="language">
                                <option value="eng">English</option>
                                <option value="ron">Romanian</option>
                            </select>
                        </div>
                        <div class="type-wrapper">
                            <label for="language">Downaload</label>
                            <select name="type" id="type">
                                <option value="docx">Docx</option>
                                <option value="txt">Txt</option>
                                <option value="json">Json</option>

                            </select>
                        </div>
                        <div class="upload-buttons">
                            <button class="start-process">Start Process</button>             
                            <i class="options-toggle fa-solid fa-gear"></i>
                        </div>
                    </div>
                 
                    <div class="options-wrapper">
                        <div class="input-group">
                            <label for="write_images">Write Images</label>
                            <input type="checkbox" checked name="write_images" id="write_images">
                        </div>
                        <div class="input-group">
                            <label for="debugging">Debugging</label>
                            <input type="checkbox" checked name="debugging" id="debugging">
                        </div>
                    </div>
                    
                    <div class="loader-wrapper">
                        <span class="loader"></span>
                        <span>Processing...</span>
                    </div>
                </div>
            </div>

            <div id="image-preview">
                
                <div class="main-text">
                        <h1>Tesseract text recognition and extraction</h1>
                        <p>This is web interface for the C++ program that uses Tesseract API 
                            for OCR-ing a custom image.                            
                        </p>
                        <p>The purpose of this project is to show the OCR process step by step and experiment with different
                            images in order to to obtain better results in terms of detection accuracy.
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

        <div id="output-wrapper">

            <div id="output-text-wrapper">
                <span>Script output:</span>
                <div id="output-text">  
                    
                </div>

            </div>
            <div class="display-wrapper">
                <span>Processed Images:</span>
                
                <div class="images-display-wrapper">
                    <div class="images-display">
                    </div>
                </div>
            </div>
           
        </div>
   
    </section>
    <div id="modal">
        <button class="close-modal">Close</button>
        <div class="modal-image-wrapper">
            <img id="modal-img" src="">
        </div>
    </div>
    <footer></footer>
    <script src="/resources/main.js?v=<?= time() ?>"></script>
</body>
</html>