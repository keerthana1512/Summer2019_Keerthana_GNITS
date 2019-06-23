from django.contrib import admin
from django.urls import path
from django.conf import settings
from django.conf.urls import include, url  # For django versions before 2.0
from django.urls import include, path  # For django versions from 2.0 and up
from matchapp import views
from matchapp import urls

urlpatterns = [
    path('admin/', admin.site.urls),
    path('', include('matchapp.urls')),
]
