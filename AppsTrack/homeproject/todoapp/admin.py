from django.contrib import admin
from todoapp.models import *

# Register your models here.
admin.site.register(Todolist)
admin.site.register(Todoitem)