#!/usr/bin/python3
import cgi
import cgitb
cgitb.enable()

# POST 데이터 읽기
form = cgi.FieldStorage()

# 'name' 필드가 존재하는지 확인
if "name" in form:
    # 'name' 필드의 값을 가져오기
    name = form["name"].value
    print("<html>")
    print("<head><title>POST Success</title></head>")
    print("<body>")
    print("<h1>Hello, {}!</h1>".format(name))
    print("</body>")
    print("</html>")
else:
    # 'name' 필드가 없을 경우
    print("<html>")
    print("<head><title>POST Failed</title></head>")
    print("<body>")
    print("<h1>No name submitted</h1>")
    print("</body>")
    print("</html>")
