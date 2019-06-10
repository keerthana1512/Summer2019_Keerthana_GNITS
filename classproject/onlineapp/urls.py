from django.urls import include, path  # For django versions from 2.0 and up
from onlineapp import views

urlpatterns = [
    path('get_my_college/', views.get_my_college),
    path('get_all_colleges/', views.get_all_colleges),
    path('get_colleges_url/', views.get_colleges_url),
    path('get_colleges_url/<int:num>/', views.get_students_marks),
    path('create_post/', views.createpost),

    #path('college<int:num>/', views.colleges_url),
    #path((?P<year>[0-9]{4}),views.
]
