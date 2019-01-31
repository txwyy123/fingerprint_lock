#coding=utf-8

from ..models import UserInfo
from app import db
from . import user_info
from flask import request, render_template
import datetime

@user_info.route('/upload', methods=['POST'])
def upload():
    form = request.form
    now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    user = UserInfo(form['id'], form['user_name'], form['role'], 0, now, now)
    db.session.add(user)
    db.session.commit()
    return "SUCCESS"

@user_info.route('/display', methods=['GET'])
def display():
    query = db.session.query(UserInfo)
    user_list = query.all()
    return render_template('user_info/user_info.html', user_list = user_list)
