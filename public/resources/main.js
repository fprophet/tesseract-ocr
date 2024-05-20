function $(selector, all = false) {
  if (all) {
    return document.querySelectorAll(selector);
  } else {
    return document.querySelector(selector);
  }
}

function pageEvents() {
  const options = $(".options-toggle");
  options ? options.addEventListener("click", _toggleOptions) : null;

  const file_input = $("input[type=file]");
  file_input ? file_input.addEventListener("change", _handleFileUpload) : null;

  const start_process = $(".start-process");
  start_process
    ? start_process.addEventListener("click", _handleStartProcess)
    : null;
}

function checkFileInput() {
  const input = $("input[type=file]");
  if (!input.files || input.files.length > 1) {
    return false;
  }

  return true;
}

function uploadRequest() {
  const form_data = new FormData();
  const file = $("input[type=file]").files[0];
  form_data.append("file", file);
  return fetch("/upload.php", {
    method: "POST",
    body: form_data,
  });
}

function processRequest(file) {
  return fetch("/ocr.php", {
    method: "POST",
    body: JSON.stringify({ file: file }),
  });
}

function _toggleOptions() {
  const wrapper = $(".options-wrapper");
  if (wrapper.style.maxHeight == "0px" || wrapper.style.maxHeight === "") {
    wrapper.style.maxHeight = "300px";
  } else {
    wrapper.style.maxHeight = "0px";
  }
}

function _handleFileUpload(event) {
  const files = event.target.files;
  if (!files || files.length > 1) {
    return false;
  }

  const preview_img = $(".upload-preview");
  preview_img.src = URL.createObjectURL(files[0]);

  $(".main-text").style.maxHeight = "0px";

  $(".preview-holder").style.maxHeight = "700px";

  $(".upload-preview").style.display = "block";

  $(".preview-name").innerHTML = `Your file: <b>${files[0].name}</b>`;
}

function _handleStartProcess() {
  if (!checkFileInput()) {
    window.alert("Please select a file first!");
    return false;
  }

  uploadRequest()
    .then((res) => res.json())
    .then((res) => {
      if (res.status == "success") {
        processRequest(res.file);
        // console.log(res);
      }
    });
}

pageEvents();
