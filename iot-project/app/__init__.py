#coding=utf-8

__author__ = 'txw'

from flask import Flask
from flask_sqlalchemy import SQLAlchemy

db = SQLAlchemy()

def create_app():
    app = Flask(__name__)
    app.config['SQLALCHEMY_DATABASE_URI'] = "mysql+pymysql://root:txwyy123@127.0.0.1/iot-project"
    app.config['SQLALCHEMY_COMMIT_TEARDOWN'] = True
    app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = True
    app.config['SQLALCHEMY_COMMIT_ON_TEARDOWN'] = True
    db.init_app(app)

    from .log import log as log_blueprint
    app.register_blueprint(log_blueprint, url_prefix='/log')
    from .user_info import user_info as user_info_blueprint
    app.register_blueprint(user_info_blueprint, url_prefix='/user_info')

    return app
