# Generated by Django 4.1.7 on 2023-04-29 22:23

from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
    ]

    operations = [
        migrations.CreateModel(
            name='Package',
            fields=[
                ('pkgID', models.BigIntegerField(primary_key=True, serialize=False)),
                ('whID', models.IntegerField(default=0)),
                ('destX', models.IntegerField(default=0)),
                ('destY', models.IntegerField(default=0)),
                ('item', models.CharField(default='Apple', max_length=256)),
                ('truckID', models.IntegerField(default=0)),
                ('userID', models.IntegerField(default=0)),
                ('evaluation', models.CharField(choices=[('excellent', 'excellent'), ('mediocre', 'mediocre'), ('disappointing', 'disappointing')], default='none', max_length=256)),
            ],
        ),
        migrations.CreateModel(
            name='Search',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('trackingNumber', models.BigIntegerField()),
            ],
        ),
        migrations.CreateModel(
            name='TRUCK',
            fields=[
                ('truckID', models.IntegerField(primary_key=True, serialize=False)),
                ('status', models.IntegerField(choices=[(0, 'idle'), (1, 'en route to warehouse'), (2, 'waiting for pickup'), (3, 'delivering')])),
            ],
        ),
        migrations.CreateModel(
            name='Profile',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('image', models.ImageField(default='default.jpg', upload_to='profile_pics')),
                ('address', models.CharField(blank=True, default='', max_length=50, verbose_name='Address')),
                ('user', models.OneToOneField(on_delete=django.db.models.deletion.CASCADE, to=settings.AUTH_USER_MODEL)),
            ],
        ),
    ]
