<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Tesseract OCR</title>
    <link rel="stylesheet" href="/resources/main.css?v=" <?= time()?>>
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
            <div id="input-wrapper">
                <input type="file" id="upload">
            </div>
            <div id="image-preview">

            </div>
        </div>
        <div id="processed-images-preview">
            
        </div>
    </section>
    
    <footer></footer>
</body>
</html>