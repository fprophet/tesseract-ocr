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

  const upload_button = $("#file-button");
  upload_button
    ? upload_button.addEventListener("click", _toggleFileButton)
    : null;

  const file_input = $("input[type=file]");
  file_input ? file_input.addEventListener("change", _toggleFileLoad) : null;
}

function _toggleOptions() {
  const wrapper = $(".options-wrapper");
  if (wrapper.style.maxHeight == "0px" || wrapper.style.maxHeight === "") {
    wrapper.style.maxHeight = "300px";
  } else {
    wrapper.style.maxHeight = "0px";
  }
}

function _toggleFileLoad(event) {
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

function _toggleFileButton() {
  const input = $("input[type=file]");
  if (!input) {
    return false;
  }

  input.click();
}

pageEvents();
