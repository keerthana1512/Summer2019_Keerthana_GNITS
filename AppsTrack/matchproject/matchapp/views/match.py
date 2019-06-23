from django.views import View
from django.urls import resolve
from matchapp.models import *
from django.shortcuts import render,redirect
from matchapp.forms import *
from django.contrib.auth.mixins import LoginRequiredMixin
from django.http import *
import operator
from django.db.models import *
class MatchView(LoginRequiredMixin,View):
    login_url = '/login/'
    def get(self,request,*args,**kwargs):
        match=Match.objects.get(match_id=kwargs.get('mpk'))
        deliveries1=Deliveries.objects.filter(m_id_id=kwargs.get('mpk')).filter(inning=1)
        deliveries2 = Deliveries.objects.filter(m_id_id=kwargs.get('mpk')).filter(inning=2)
        runners=Deliveries.objects.values('batsman').annotate(Sum('total_runs')).filter(m_id_id=kwargs.get('mpk')).order_by('-total_runs__sum')
        wickets=Deliveries.objects.values('bowler').filter(m_id_id=kwargs.get('mpk')).exclude(dismissal_kind=None).annotate(Count('player_dismissed')).order_by('-player_dismissed__count')
        # return HttpResponse(wickets)
        return render(
            request,
            template_name='matchapp/match_details.html',
            context={
                'wickets':wickets,
                'runners':runners,
                'deliveries1':deliveries1,
                'deliveries2': deliveries2,
                'match':match,
                'title':'Match details | Match App'
            }
        )
