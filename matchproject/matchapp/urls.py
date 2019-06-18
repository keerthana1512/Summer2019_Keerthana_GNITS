from matchapp.views import *
from django.contrib import admin
from django.urls import path

urlpatterns = [
    path('seasons/', SeasonsView.as_view(), name='all_seasons'),
    path('seasons/<int:pk>/', SeasonsView.as_view(), name='season_details'),
    path('seasons/<int:pk>/match/<int:mpk>/', MatchView.as_view(), name='match_details'),

    path('signup/', SignupView.as_view(), name='signup'),
    path('login/', LoginView.as_view(), name='login'),
    path('logout/', logout_user, name='logout')

]
