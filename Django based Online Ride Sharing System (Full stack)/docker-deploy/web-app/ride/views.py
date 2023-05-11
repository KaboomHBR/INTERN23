from django.shortcuts import render, redirect, get_object_or_404
from django.http import HttpResponse, Http404, HttpResponseRedirect
# Create your views here.
from django.contrib import messages
from django.core.mail import send_mail
from datetime import datetime
from django.utils import timezone
from .models import *
from .forms import *

from django.views.generic import (ListView, DetailView, CreateView, UpdateView, DeleteView)

from django.contrib.auth.mixins import LoginRequiredMixin, UserPassesTestMixin
from django.contrib.auth.decorators import permission_required, login_required

def index_status(request):
    return HttpResponse("this is the view of App: ride/status")


def register(request):
    if request.method == 'POST':
        form = CreateUserForm(request.POST)
        if form.is_valid():
            form.save()
            username = form.cleaned_data.get('username')
            messages.success(request, f'Congrats! Your account was created, please log in and enjoy!')
            return redirect('login')
    else:
        form = CreateUserForm()
    return render(request, 'ride/register.html', {'form': form})


@login_required
def driver_info(request):
    if request.method == 'POST':
        u_form = UpdateUserForm(request.POST, instance=request.user)
        p_form = UpdateDriverForm(request.POST,instance=request.user.ride_driver)
        if u_form.is_valid() and p_form.is_valid():
            u_form.save()
            p_form.save()
            messages.success(request, f'Your account has been updated!')
            return redirect('driver_info')
    else:
        u_form = UpdateUserForm(instance=request.user)
        p_form = UpdateDriverForm(instance=request.user.ride_driver)
    context = {
        'u_form': u_form,
        'p_form': p_form
    }
    return render(request, 'ride/driver_info.html', context)
                                                            

def home(request):
    return render(request, 'ride/home.html')


@login_required
def rideowner_home(request):
    return render(request, 'ride/rideowner_home.html')

class RideOwnerCreateView(LoginRequiredMixin, CreateView):
    model = Ride_owner
    fields = ['destination', 'arrival_time', 'num_passengers', 'vehicle_type', 'sharing', 'special_requests']
    def form_valid(self, form):
        form.instance.owner = self.request.user
        return super().form_valid(form)

class RideOwnerUpdateView(LoginRequiredMixin, UserPassesTestMixin, UpdateView):
    model = Ride_owner
    fields = ['destination', 'arrival_time', 'num_passengers', 'vehicle_type', 'sharing', 'special_requests']
    def form_valid(self, form):
        form.instance.owner = self.request.user
        return super().form_valid(form)
    def test_func(self):
        ride_owner = self.get_object()
        if self.request.user == ride_owner.owner:
            return True
        return False

class RideOwnerDeleteView(LoginRequiredMixin, UserPassesTestMixin, DeleteView):
    model = Ride_owner
    success_url = '/rideowner/status/'
    def test_func(self):
        ride_owner = self.get_object()
        if self.request.user == ride_owner.owner:
            return True
        return False
'''
class RideOwnerPastListView(ListView):
    template_name = 'ride/driverpast_list.html'
    def get_queryset(self):
        return Rideowner.objects.filter(status='complete', owner = self.request.user).order_by('arrive_date')
'''

class RideOwnerListView(ListView):
    template_name = 'ride/rideowner_list.html'
    def get_queryset(self):
        return Ride_owner.objects.filter(owner=self.request.user).order_by('arrival_time')


def ridedriver_home(request):
    driverr = Ride_driver.objects.filter(driver = request.user.id).first()
    if driverr.license_plate == '':
        return render(request, 'ride/driver_register.html')
    return render(request, 'ride/driver_home.html')

@login_required
def confirm(request, rid):
    driver = Ride_driver.objects.filter(driver = request.user.id).first()
    ride = Ride_owner.objects.filter(pk = rid).first();
    ride.status = 'confirmed';
    ride.driver_name= request.user.username
    ride.license_plate = driver.license_plate
    ride.save();
    send_mail(
        'Here is the message',
        'As a ride-owner, your ride has been comfirmed',
        '894306698@qq.com',
        [ride.owner.email],
        fail_silently=False,
    )
    sharer = User.objects.filter(username=ride.sharer).first()
    if sharer:
        send_mail(
            'Here is the message',
            'As a ride-sharer, your ride has been comfirmed',
            '894306698@qq.com',
            [sharer.email],
            fail_silently=False,
        )
    return render(request, 'ride/driver_home.html')

@login_required
def complete(request, rid):
    ride = Ride_owner.objects.filter(pk = rid).first();
    ride.status = 'completed';
    ride.save();
    return render(request, 'ride/driver_home.html')


class RideDriverSearchView(ListView):
    template_name = 'ride/driver_search.html'
    def get_queryset(self):
        return Ride_owner.objects.filter(status__in=['open'],
                                        num_passengers__lte=self.request.user.ride_driver.num_passengers,
                                        vehicle_type__in=['--', self.request.user.ride_driver.vehicle_type],
                                        special_requests__in=['', self.request.user.ride_driver.special_info]).exclude(owner=self.request.user).order_by('arrival_time')

    
class RideDriverStatusView(ListView):
    template_name = 'ride/driver_status.html'
    def get_queryset(self):
        return Ride_owner.objects.filter(status__in=['confirmed', 'completed'], driver_name = self.request.user.username).exclude(owner=self.request.user).order_by('arrival_time')

def ridesharer_home(request):
    return render(request, 'ride/ridesharer_home.html')

class RideSharerSearchView(LoginRequiredMixin, CreateView):
    model = Ride_sharer
    fields = ['destination', 'earliest_arrival_time', 'latest_arrival_time','num_passengers']
    def form_valid(self, form):
        form.instance.sharer = self.request.user
        return super().form_valid(form)

class RideSharerResultView(ListView):
    template_name = 'ride/ridesharer_result.html'
    def get_queryset(self):
        share = self.request.user.ride_sharer_set.last()
        return Ride_owner.objects.filter(sharing=True,
                                        destination=share.destination,
                                        status='open',
                                        arrival_time__gte=share.earliest_arrival_time,
                                        arrival_time__lte=share.latest_arrival_time,
        ).exclude(owner=self.request.user).order_by('arrival_time')

class RideSharerStatusView(ListView):
    template_name = 'ride/ridesharer_status.html'
    def get_queryset(self):
        share = self.request.user.ride_sharer_set.last()
        return Ride_owner.objects.filter(sharer=self.request.user.username).exclude(owner=self.request.user).order_by('arrival_time')

@login_required     
def join(request, rid):
    ride = Ride_owner.objects.filter(pk = rid).first();
    share_person = Ride_sharer.objects.filter(sharer = request.user.id).last()
    ride.status = 'open'
    ride.sharer = request.user.username
    ride.num_passengers = ride.num_passengers + share_person.num_passengers
    ride.save();
    return render(request, 'ride/ridesharer_home.html')

@login_required
def cancel(request, rid):
    ride = Ride_owner.objects.filter(pk = rid).first();
    share_person = Ride_sharer.objects.filter(sharer = request.user.id).last()
    ride.status = 'open'
    ride.sharer = ''
    ride.num_passengers = ride.num_passengers - share_person.num_passengers
    ride.save();
    return render(request, 'ride/ridesharer_home.html')

                                                                
