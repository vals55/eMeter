function flip() {
    var b = document.querySelector('.update');
    if (b.style.display == 'block') b.style.display = 'none';
    else b.style.display = 'block'
}

function _(b) {
    return document.getElementById(b)
}
function upd() {
    const xhr = new XMLHttpRequest();
    xhr.open('GET', '/update');
    xhr.send()
}
function send(url, milli, _process) {
    const xhr = new XMLHttpRequest();
    xhr.open('GET', url);
    xhr.timeout = milli;
    xhr.onreadystatechange = function(a) {
        if (xhr.readyState === 4 && xhr.status === 200) {
            _process(xhr.responseText)
        }
    };
    xhr.send()
}
window.onload = function() {
    function _refresh() {
        function _process(msg) {
            var obj = JSON.parse(msg);
            Object.keys(obj).forEach(function(id) {
                if (id.startsWith('inner')) {
                    var t = id.slice(6);
                    if (_(t)) {
                        _(t).innerHTML = obj[id];
                    }
                }
                if (id.startsWith('value')) {
                    var t = id.slice(6);
                    if (_(t)) {
                        _(t).value = obj[id];
                    }
                }
                if (id.startsWith('style')) {
                    var t = id.slice(6);
                    if (_(t)) {
                        _(t).style = obj[id];
                    }
                }
            })
        }
        send('/states', 250, _process);
    }
    _refresh();
    var d = setInterval(_refresh, 500);
}