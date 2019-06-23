from django.shortcuts import render
from django.http import HttpResponse
from onlineapp import templates
from django.shortcuts import render
from onlineapp.models import *

def hello_world(request):
    #return render(, {})
    f=open("C:\PythonCourse\summer\classproject\onlineapp\hello.html")
    html_content=f.read()
    foo=request.headers
    return HttpResponse(list(foo.items()))

    '''val=request.headers['foo']
    return HttpResponse(val)
    '''

def get_my_college(request):
    return HttpResponse(College.objects.values('name').filter(acronym='cvr'))

def get_all_colleges(request):
    colleges=College.objects.values_list('name','acronym')
    return render(request,"all_colleges.html",{'college_list':colleges})

def get_colleges_url(request):
    colleges=College.objects.values_list('id','acronym')
    return render(request, "all_colleges_url.html", {'college_list': colleges})

def get_students_marks(request,num):
    students=Student.objects.values_list('id','name','mocktest1__total').filter(college_id=num)
    return render(request, "students_marks.html", {'student_list': students})

def createpost(request):
    def createpost(request):
        if request.method == 'POST':
            if request.POST.get('namw'):# and request.POST.get('content'):
                c = College()
                c.name = request.POST.get('name')
                c.location = request.POST.get('location')
                c.acronym=request.POST.get('acronym')
                c.contact = request.POST.get('contact')
                c.save()
                return HttpResponse("<H1>done</H1")
                return render(request, 'posts/create.html')

        else:
            return HttpResponse("done not")
            return render(request, 'posts/create.html')