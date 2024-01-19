#!/usr/bin/python3

import cgi

print("<html>")
print("<head><title>Simple CGI Query</title></head>")
print("<body>")

form = cgi.FieldStorage()

if "a" in form and "b" in form:
    if (form["a"].value.isdigit()):
        a = int(form["a"].value)
    else:
        a = 0
    if (form["b"].value.isdigit()):
        b = int(form["b"].value)
    else:
        b = 0
    result = a * b
    print("<p>Your answer is...</p>")
    print("<h1>{} * {} = {}</h1>".format(a, b, result))
else:
    print("<h1>Invalid query</h1>")
    print("<p>Please enter 'a' and 'b' value in query string</p>")

print("</body>")
print("</html>")
