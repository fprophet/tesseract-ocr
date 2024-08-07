let interval = false;

function $(selector, all = false) {
  if (all) {
    return document.querySelectorAll(selector);
  } else {
    return document.querySelector(selector);
  }
}

function $create(tag, id = false, cls = false, text = false) {
  const element = document.createElement(tag);
  if (id) {
    element.setAttribute("id", id);
  }
  if (cls) {
    cls.forEach(function (elem) {
      element.classList.add(elem);
    });
  }
  if (text) {
    element.innerHTML = text;
  }
  return element;
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

  const close_modal = $(".close-modal");
  if (close_modal) {
    close_modal.addEventListener("click", _handleCloseModal);
  }

  const boxes = $("input[type=checkbox]", true);
  if (boxes) {
    boxes.forEach(function (bx) {
      bx.addEventListener("click", _handleBoxClick);
    });
  }
}

function checkFileInput() {
  const input = $("input[type=file]");
  if (!input.files || input.files.length < 1) {
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

function processRequest(payload) {
  return fetch("/ocr.php", {
    method: "POST",
    body: JSON.stringify(payload),
  });
}

function loading(display = true) {
  if (display) {
    $(".loader-wrapper").style.display = "flex";
  } else {
    $(".loader-wrapper").style.display = "none";
  }
}

function _toggleOptions() {
  const wrapper = $(".options-wrapper");
  if (wrapper.style.maxHeight == "0px" || wrapper.style.maxHeight === "") {
    wrapper.style.maxHeight = "300px";
  } else {
    wrapper.style.maxHeight = "0px";
  }
}

function _handleBoxClick(event) {
  if (event.target.hasAttribute("checked")) {
    event.target.removeAttribute("checked");
  } else {
    event.target.setAttribute("checked", "checked");
  }
}

function _handleCloseModal(event) {
  $("#modal").style.display = "none";
}

function _handleModalImage(event) {
  const img = event.target.closest("img");

  $("#modal-img").src = img.src;
  $("#modal").style.display = "block";
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

function getPayload() {
  let payload = {};
  payload.language = $("#language").value;
  if ($("#write_images").hasAttribute("checked")) {
    payload.write_images = true;
  }
  if ($("#debugging").hasAttribute("checked")) {
    payload.debugging = true;
  }
  payload.download = $("#type").value;

  return payload;
}

function _handleStartProcess() {
  if (!checkFileInput()) {
    window.alert("Please select a file first!");
    return false;
  }
  $("#output-text").innerHTML = "";
  $(".images-display").innerHTML = "";
  loading();

  uploadRequest()
    .then((res) => res.json())
    .then((res) => {
      if (res.status == "success") {
        interval = true;
        updateData();
        const payload = getPayload();
        payload.file = res.file;
        processRequest(payload)
          .then((res) => res.json())
          .then((res) => {
            interval = false;
            loading(false);
            if (res["link"]) {
              createDownloadLink(res["link"]);
            }
          });
        // console.log(res);
      }
    });
}

function createDownloadLink(link) {
  const a = document.createElement("a");
  a.href = link;
  a.innerHTML = link;
  $(".file-input-wrapper").appendChild(a);
}

function udpateDatax() {
  interval = 100;
  let udpdate = setInterval(function () {
    if (!interval) {
      clearInterval(update);
    }
  }, interval);
}

function displayImage(image) {
  fetch("/processed-images/" + image)
    .then((res) => res.blob())
    .then((blob) => {
      const imageUrl = URL.createObjectURL(blob);

      const wrapper = $(".images-display");

      const holder = $create("div", false, ["img-holder"]);

      const name_span = $create("span", false, ["img-name"], image);

      const img = $create("img");

      img.src = imageUrl;

      holder.appendChild(img);
      holder.appendChild(name_span);

      holder.addEventListener("click", _handleModalImage);
      wrapper.appendChild(holder);
    });
}

function updateData() {
  fetch("/progress.php?progress=process", {
    mehtod: "GET",
  })
    .then((res) => res.json())
    .then((res) => {
      if (res["status"] !== "empty") {
        for (i = 0; i < res["data"].length; i++) {
          if (!res["data"][i]) {
            continue;
          }
          $("#output-text").innerHTML += res["data"][i] + " <br>";
          $("#output-text").scrollTop = $("#output-text").scrollHeight;
          if (res["data"][i].indexOf("--image:") > -1) {
            let image = res["data"][i].split(":")[1].trim();
            displayImage(image);
          }
        }
      }

      if (interval) {
        updateData();
      }
    });
}

pageEvents();
