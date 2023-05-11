from django.db import models
from django.contrib.auth.models import User
from django.urls import reverse

class Profile(models.Model):   
    user = models.OneToOneField(User, on_delete = models.CASCADE)
    image = models.ImageField(default = 'default.jpg', upload_to = 'profile_pics')
    address = models.CharField(max_length = 50, verbose_name = "Address", default = '', blank = True)

    def __str__(self):
        return f'{self.user.username} Profile'


EVALUATION_STATUS = (
    ("excellent", "excellent"),
    ("mediocre", "mediocre"),
    ("disappointing", "disappointing")
)

class Pkg(models.Model):
    pkgid = models.BigIntegerField(primary_key=True)
    whid = models.IntegerField(default = 0)
    destx = models.IntegerField(default = 0)
    desty = models.IntegerField(default = 0)
    item = models.CharField(max_length = 256, default = 'Apple')
    truckid = models.IntegerField(default = 0)
    userid = models.IntegerField(default = 0)
    evaluation = models.CharField(max_length = 256, choices = EVALUATION_STATUS, default = 'none', blank=True, null=True)

    class Meta:
        db_table = 'pkg'

    def __str__(self):
        return f'{self.pkgid}'

    #When submit, the webpage will go to the certain place
    def get_absolute_url(self):
        return reverse('package-list')

TRUCK_STATUS = (
    (0, "idle"),
    (1, "en route to warehouse"),
    (2, "waiting for pickup"),
    (3, "delivering")
)

class Truck(models.Model):
    truckid = models.IntegerField(primary_key=True)
    status = models.IntegerField(choices = TRUCK_STATUS)
    truckx = models.IntegerField(default = 0)
    trucky = models.IntegerField(default = 0)
    class Meta:
        db_table = 'truck'

    def __str__(self):
        return f'{self.truckid}'

class userr(models.Model):
    userid = models.IntegerField(primary_key=True)

    class Meta:
        db_table = 'userr'

class Search(models.Model):
    trackingNumber = models.BigIntegerField()

    def __str__(self):
        return f'{self.trackingNumber}'
    #When submit, the webpage will go to the certain place
    def get_absolute_url(self):
        return reverse('search-list')