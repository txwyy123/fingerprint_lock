from flask import Blueprint

user_info = Blueprint('user_info', __name__)

from . import views
