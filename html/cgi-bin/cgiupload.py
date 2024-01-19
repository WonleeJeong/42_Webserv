#!/usr/bin/python3

import cgi
import os
from urllib.parse import quote

# 허용된 파일 확장자
ALLOWED_EXTENSIONS = {'txt', 'html'} # 파일 형식

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

form = cgi.FieldStorage()

if "filename" in form:
    fileitem = form['filename']

    path = "./html/upload/"

    isExist = os.path.exists(path)

    if not isExist:
        os.makedirs(path)

    if fileitem.filename:
        if allowed_file(fileitem.filename):
            encoded_filename = quote(fileitem.filename, encoding='utf-8')
            fn = os.path.basename(encoded_filename)
            with open(os.path.join(path, fn), 'wb') as f:
                f.write(fileitem.file.read())
            title = "Upload Success"
            message = "The file '{}' was uploaded successfully".format(fn)
        else:
            title = "Upload Failed"
            message = "Invalid file extension. Only the following file extensions are allowed: {}".format(', '.join(ALLOWED_EXTENSIONS))
    else:
        title = "Upload Failed"
        message = "No file was uploaded"

else:
    title = "Upload Failed"
    message = "No file was uploaded"

print()
print("""\
<html>
<head><title>{}</title></head>
<body>
<h1>{}</h1>
<p>{}</p>
</body>
</html>
""".format(title, title, message))