from django.db.models.signals import post_save
from django.contrib.auth.models import User
from django.dispatch import receiver
#from .models import Driver_info
from .models import Ride_driver

@receiver(post_save, sender=User)
#def create_driver_info(sender, instance, created, **kwargs):
def create_ride_driver(sender, instance, created, **kwargs):
    if created:
        Ride_driver.objects.create(driver=instance)
#        Driver_info.objects.create(user=instance)
        

@receiver(post_save, sender=User)
#def save_driver_info(sender, instance, **kwargs):
def save_ride_driver(sender, instance, **kwargs):
    #instance.driver_info.save()
    instance.ride_driver.save()
                            
