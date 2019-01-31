#coding=utf-8

from ..models import Log, UserInfo
from app import db
from . import log
from flask import request, render_template
import datetime

@log.route('/upload', methods=['POST'])
def upload():
    form = request.form
    now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    log = Log(form['log'], now)
    db.session.add(log)

    params = form['log'].split('_')
    db.session.query(UserInfo).filter(UserInfo.id == int(params[0]))\
        .update({"open_count": UserInfo.open_count+1, "update_time": now})

    db.session.commit()
    db.session.close()
    return "SUCCESS"

@log.route('/display', methods=['GET'])
def display():
    query = db.session.query(Log)
    log_list = query.all()
    return render_template('log/log_list.html', log_list=log_list)
