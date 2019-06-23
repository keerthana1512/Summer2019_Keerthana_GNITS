from django.core.paginator import Paginator
from django.views import View
from matchapp.models import *
from django.shortcuts import render
from django.contrib.auth.mixins import LoginRequiredMixin

class SeasonsView(LoginRequiredMixin,View):
    login_url = '/login/'
    def get(self,request,*args,**kwargs):
        year = Match.objects.values_list('season').distinct().order_by('-season')
        if kwargs:
            seasons=Match.objects.all().filter(season=kwargs.get('pk'))
        else:
            seasons = Match.objects.all().filter(season=year[0][0])
        paginator = Paginator(seasons, 8)
        page = request.GET.get('page')
        seasons = paginator.get_page(page)
        return render(
            request,
            template_name='matchapp/season_details.html',
            context={
                'cur_season':kwargs.get('pk'),
                'years':year,
                'seasons': seasons,
                'title': 'Matches | Match App'
            }
        )