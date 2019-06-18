from django.views import View
from django.urls import resolve
from matchapp.models import *
from django.shortcuts import render,redirect
from matchapp.forms import *
from django.contrib.auth import authenticate,login,logout
from django.shortcuts import render
from django.contrib import messages
from django.contrib.auth.models import User
from django.http import HttpResponse


def logout_user(request):
    logout(request)
    return redirect('login')


class SignupView(View):
    def get(self,request):
        form=SignupForm()
        return render(
            request,
            template_name='matchapp/signup_html.html',
            context={
                'signupform':form
            }
        )
    def post(self,request):
        form=SignupForm(request.POST)
        if form.is_valid():
            user=User.objects.create_user(**form.cleaned_data)
            user.save()
            user=authenticate(
                request,
                username=form.cleaned_data['username'],
                password=form.cleaned_data['password'],
            )
            if user is not None:
                login(request,user)
                return redirect('all_seasons')
        else:
            return render(
                request,
                template_name='matchapp/signup_html.html',
                context={
                    'signupform': form
                }
            )


class LoginView(View):
    def get(self,request):
        form=LoginForm()
        return render(
            request,
            template_name='matchapp/login_html.html',
            context={
                'loginform':form,
            }
        )

    def post(self,request):
        form=LoginForm(request.POST)
        if form.is_valid():
            user=authenticate(
                request,
                username=form.cleaned_data['username'],
                password=form.cleaned_data['password'],
            )
            if user is not None:
                login(request, user)
                return redirect('all_seasons')
            else:
                return HttpResponse('invalid')#messages.error(request,'invalid credentials')
        return render(
            request,
            template_name='matchapp/login_html.html',
            context={
                'loginform':form
            }
        )
