#!/usr/bin/python3

import cgi

print("<html>")
print("<head><title>Simple CGI POST</title></head>")
print("<body>")
print("<h1>Insert a name</h1>")
print("<form method='post' action='/cgi-bin/cgipostres.py'>")
print("<p><input type='text' name='name'></p>")
print("<p><input type='submit' value='submit name'></p>")
print("</form>")
print("</body>")
print("</html>")
