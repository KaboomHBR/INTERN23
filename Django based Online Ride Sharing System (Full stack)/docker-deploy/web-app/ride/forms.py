from django import forms
from .models import *
from django.contrib.auth.models import User
from django.contrib.auth.forms import UserCreationForm
from datetime import datetime, timedelta, tzinfo

class CreateUserForm(UserCreationForm):
    email = forms.EmailField()
    class Meta:
        model = User
        fields = ['username', 'email', 'password1', 'password2']

class UpdateUserForm(forms.ModelForm):
    email = forms.EmailField()
    class Meta:
        model = User
        fields = ['username', 'email']

class UpdateDriverForm(forms.ModelForm):
    class Meta:
        #model = Driver_info
        model = Ride_driver
        fields = ['license_plate', 'num_passengers', 'vehicle_type', 'special_info']
                            
