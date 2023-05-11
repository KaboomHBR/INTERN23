from django.shortcuts import render, redirect
from .models import Profile, Pkg, Search, Truck, userr
from django.contrib.auth.mixins import LoginRequiredMixin, UserPassesTestMixin
from django.views.generic import (
    ListView, DetailView, CreateView, UpdateView, DeleteView)
from django.core.mail import send_mail
from django.contrib import messages
from .forms import UserRegisterForm, UserUpdateForm, ProfileUpdateForm
from django.contrib.auth.decorators import login_required
from django.http import Http404
from django.contrib.auth.models import User

from django.db import models
from django.db.models.signals import post_save
from django.dispatch import receiver


def home(request):
    return render(request, 'ups/home.html')


def create_userr(sender, instance, created, **kwargs):
    if created:
        try:
            username_int = int(instance.username)
            userr.objects.create(userid=username_int)
        except ValueError:
            raise Http404


models.signals.post_save.connect(create_userr, sender=User)


class SearchCreateView(CreateView):
    model = Search
    fields = ['trackingNumber']

    def form_valid(self, form):
        return super().form_valid(form)


class SearchListView(ListView):
    template_name = 'ups/search_list.html'

    def get_queryset(self):
        s = Search.objects.last()
        return Pkg.objects.filter(pkgid=s.trackingNumber)

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context['trucks'] = Truck.objects.all()
        return context


class PackageListView(ListView):
    model = Pkg
    template_name = 'ups/package_list.html'

    def get_queryset(self):
        return Pkg.objects.filter(userid=int(self.request.user.username))

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context['trucks'] = Truck.objects.all()
        return context


class PackageUpdateView(LoginRequiredMixin, UserPassesTestMixin, UpdateView):
    model = Pkg
    fields = ['destx', 'desty']
    template_name = 'ups/package_form.html'

    def form_valid(self, form):
        form.instance.owner = self.request.user.username
        response = super().form_valid(form)
        self.send_email()
        return response

    def test_func(self):
        package = self.get_object()
        if int(self.request.user.username) == package.userid:
            return True
        return False

    def send_email(self):
        package = self.get_object()
        owner = User.objects.get(username=str(package.userid))
        owner_email = owner.email
        send_mail(
            subject='Package Info Update',
            message=f'Your package ({package.pkgid}) information has been successfully updated.',
            from_email='a1453737821@163.com',
            recipient_list=[owner_email],
            fail_silently=False,
        )


class PackageEvaUpdateView(LoginRequiredMixin, UserPassesTestMixin, UpdateView):
    model = Pkg
    fields = ['evaluation']
    template_name = 'ups/package_form.html'

    def form_valid(self, form):
        form.instance.owner = self.request.user.username
        return super().form_valid(form)

    def test_func(self):
        package = self.get_object()
        if int(self.request.user.username) == package.userid:
            return True
        return False

# Function 1: create an account


def register(request):
    if request.method == 'POST':
        form = UserRegisterForm(request.POST)
        if form.is_valid():
            form.save()
            messages.success(request, f'Account created. Login now.')
            return redirect('login')
        else:
            raise Http404
    else:
        form = UserRegisterForm()
    return render(request, 'ups/register.html', {'form': form})

# Function 2: update user profile


@login_required  # if not satisfied, the user will be redirected to the login page
def profile(request):
    if request.method == 'POST':
        # UserUpdateForm is used to update the built-in User model
        u_form = UserUpdateForm(request.POST, instance=request.user)
        # ProfileUpdateForm is used to update a custom Profile model
        p_form = ProfileUpdateForm(
            request.POST, request.FILES, instance=request.user.profile)
        if u_form.is_valid() and p_form.is_valid():
            u_form.save()
            p_form.save()
            messages.success(request, f'Account updated.')
            return redirect('profile')
        else:
            raise Http404
    else:
        u_form = UserUpdateForm(instance=request.user)
        p_form = ProfileUpdateForm(instance=request.user.profile)
    context = {
        'u_form': u_form,
        'p_form': p_form
    }
    return render(request, 'ups/profile.html', context)
