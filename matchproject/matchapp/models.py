from django.db import models

# Create your models here.

class Match(models.Model):
    match_id=models.IntegerField()
    season = models.IntegerField(null=True)
    city = models.CharField(max_length=128,null=True)
    date=models.DateField(max_length=128,null=True)
    team1 = models.CharField(max_length=128,null=True)
    team2 = models.CharField(max_length=128,null=True)
    toss_winner = models.CharField(max_length=128,null=True)
    toss_decision = models.CharField(max_length=128,null=True)
    result=models.CharField(max_length=128,null=True)
    dl_applied=models.CharField(max_length=128,null=True)
    winner=models.CharField(max_length=128,null=True)
    win_by_runs=models.IntegerField(null=True)
    win_by_wickets=models.IntegerField()
    player_of_match=models.CharField(max_length=128,null=True)
    venue=models.CharField(max_length=128,null=True)
    umpire1=models.CharField(max_length=128,null=True)
    umpire2=models.CharField(max_length=128,null=True)
    umpire3=models.CharField(max_length=128,null=True)


class Deliveries(models.Model):
    inning=models.IntegerField(max_length=128,null=True)
    batting_team=models.CharField(max_length=128,null=True)
    bowling_team=models.CharField(max_length=128,null=True)
    over=models.IntegerField(max_length=128,null=True)
    ball=models.IntegerField(max_length=128,null=True)
    batsman=models.CharField(max_length=128,null=True)
    non_striker=models.CharField(max_length=128,null=True)
    bowler=models.CharField(max_length=128,null=True)
    is_super_over=models.IntegerField(max_length=128,null=True)
    wide_runs=models.IntegerField(max_length=128,null=True)
    bye_runs=models.IntegerField(max_length=128,null=True)
    legbye_runs=models.IntegerField(max_length=128,null=True)
    noball_runs=models.IntegerField(max_length=128,null=True)
    penalty_runs=models.IntegerField(max_length=128,null=True)
    batsman_runs=models.IntegerField(max_length=128,null=True)
    extra_runs=models.IntegerField(max_length=128,null=True)
    total_runs=models.IntegerField(null=True)
    player_dismissed=models.CharField(max_length=128,null=True)
    dismissal_kind=models.CharField(max_length=128,null=True)
    fielder=models.CharField(max_length=128,null=True)

    m_id=models.ForeignKey(Match,on_delete=models.CASCADE)

