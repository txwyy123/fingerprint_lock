#coding=utf-8

from app import db

class UserInfo(db.Model):
    __tablename__ = 'user_info'
    id = db.Column(db.Integer, primary_key=True)
    user_name = db.Column(db.String(64))
    role = db.Column(db.Integer)
    open_count = db.Column(db.Integer)
    create_time = db.Column()
    update_time = db.Column()

    def __init__(self, id, user_name, role, open_count, create_time, update_time):
        self.id = id
        self.user_name = user_name
        self.role = role
        self.open_count = open_count
        self.create_time = create_time
        self.update_time = update_time

class Log(db.Model):
    __tablename__ = 'op_log'
    id = db.Column(db.Integer, primary_key=True)
    log = db.Column(db.String(1000))
    create_time = db.Column()

    def __init__(self, log, create_time):
        self.log = log
        self.create_time = create_time
