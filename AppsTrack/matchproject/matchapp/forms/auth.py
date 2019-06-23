from django import forms
from matchapp.models import *


class SignupForm(forms.Form):
    first_name=forms.CharField(
        required=True,
        max_length=20,
        widget=forms.TextInput(attrs={'class':'input','placeholder':'First Name'}),
    )
    last_name = forms.CharField(
        required=True,
        max_length=20,
        widget=forms.TextInput(attrs={'class': 'input', 'placeholder': 'Last Name'}),
    )
    username = forms.CharField(
        required=True,
        max_length=20,
        widget=forms.TextInput(attrs={'class': 'input', 'placeholder': 'User Name'}),
    )
    password = forms.CharField(
        required=True,
        max_length=20,
        widget=forms.PasswordInput(attrs={'class': 'input', 'placeholder': 'Password'}),
    )

class LoginForm(forms.Form):
    username = forms.CharField(
        required=True,
        max_length=20,
        widget=forms.TextInput(attrs={'class': 'input', 'placeholder': 'User Name'}),
    )
    password = forms.CharField(
        required=True,
        max_length=20,
        widget=forms.PasswordInput(attrs={'class': 'input', 'placeholder': 'Password'}),
    )


