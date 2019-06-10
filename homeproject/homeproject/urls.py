"""homeproject URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/2.2/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path
from django.conf import settings
from django.conf.urls import include, url  # For django versions before 2.0
from django.urls import include, path  # For django versions from 2.0 and up
from todoapp import views
'''
urlpatterns = [
    path('admin/', admin.site.urls),
]'''
if settings.DEBUG:
    import debug_toolbar
    urlpatterns = [
        path('admin/', admin.site.urls),
        path('hello/',views.hello_world),
        #path('__debug__/', include(debug_toolbar.urls)),
        # For django versions before 2.0:
        url(r'^__debug__/', include(debug_toolbar.urls)),
   ]# + urlpatterns
