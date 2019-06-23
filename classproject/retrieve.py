import os
import django

os.environ.setdefault("DJANGO_SETTINGS_MODULE","classproject.settings")
django.setup()

from onlineapp.models import *
#c=College.objects.all()
#print(c)

#c=College.objects.values('acronym','contact')
#print(c)

#c=College.objects.all()
#print(c)
from django.db.models import *
c=College.objects.all()
c.values('location').annotate(Count('location'))
print(c)

c.values('location').annotate(Count('location')).order_by(Count('location'))

s=Student.objects.values('college__location').annotate(Count('id'))

MockTest1.objects.values('total','student__name',"student__college__name")
