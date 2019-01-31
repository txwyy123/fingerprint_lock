#!/usr/bin/env python
#coding=utf-8

from app import create_app
from tornado.wsgi import WSGIContainer
from tornado.httpserver import HTTPServer
from tornado.ioloop import IOLoop

app = create_app()

if __name__ == '__main__':
    app.run(host='0.0.0.0',debug=True,port=10000)
