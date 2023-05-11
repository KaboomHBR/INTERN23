from django.db import models
from django.utils import timezone
from django.contrib.auth.models import User
from django.urls import reverse
# Create your models here.

VEHICLE_TYPE_OWNER = (
    ("SEDAN", "SEDAN"),
    ("SUV", "SUV"),
    ("VAN", "VAN"),
    ("--", "--"),
    )

VEHICLE_TYPE_DR = (
        ("SEDAN", "SEDAN"),
        ("SUV", "SUV"),
        ("VAN", "VAN"),
    )

STATUS_TYPE = (
    ("open", "open"),
    ("confirmed", "confirmed"),
    ("completed", "completed"),
)

class Ride_owner(models.Model):
    owner = models.ForeignKey(User, on_delete=models.CASCADE)
    destination = models.CharField(max_length = 100)
    arrival_time = models.DateTimeField(help_text='Format: 2023-02-23 12:00')
    num_passengers = models.PositiveIntegerField(default=4)
    vehicle_type = models.CharField(max_length = 20, choices = VEHICLE_TYPE_OWNER, default = "--")
    sharing = models.BooleanField()
    sharer = models.CharField(default='', max_length=50, blank=True)
    special_requests = models.CharField(max_length = 200, blank=True)
    status = models.CharField(max_length=20, choices=STATUS_TYPE, default="open")
    driver_name = models.CharField(default='', max_length=50, blank=True)
    license_plate = models.CharField(default='', max_length=50, blank=True)
    def __str__(self):
        return f'{self.owner.username} Ride_owner'
    def get_absolute_url(self):
        return reverse('rideowner-status')

class Ride_sharer(models.Model):
    sharer = models.ForeignKey(User, on_delete=models.CASCADE)
    destination = models.CharField(max_length = 100)
    earliest_arrival_time = models.DateTimeField(help_text='Format: 2023-02-23 12:00')
    latest_arrival_time = models.DateTimeField(help_text='Format: 2023-02-23 13:00')
    num_passengers = models.PositiveIntegerField(default=1)
    def __str__(self):
        return f'{self.sharer.username} Ride_sharer'
    def get_absolute_url(self):
        return reverse('ridesharer-result')
    
class Ride_driver(models.Model):
    driver = models.OneToOneField(User, on_delete=models.CASCADE)
    license_plate = models.CharField(max_length = 10, default = '', blank = True)
    num_passengers = models.PositiveIntegerField(default = 1)
    vehicle_type = models.CharField(max_length = 20, choices = VEHICLE_TYPE_DR, default = "SEDAN")
    special_info = models.TextField(default = '', blank = True)
    def __str__(self):
        return f'{self.driver.username} Ride_driver'
                                    

