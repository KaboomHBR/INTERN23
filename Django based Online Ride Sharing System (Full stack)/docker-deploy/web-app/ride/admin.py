from django.contrib import admin

# Register your models here.

from .models import *#Ride_owner, Ride_sharer, Driver_info

admin.site.register(Ride_owner)
admin.site.register(Ride_sharer)
#admin.site.register(Driver_info)
admin.site.register(Ride_driver)
