# Generated by Django 4.1.5 on 2023-04-30 01:57

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('ups', '0002_pkg_userr_delete_package_and_more'),
    ]

    operations = [
        migrations.AddField(
            model_name='truck',
            name='truckx',
            field=models.IntegerField(default=0),
        ),
        migrations.AddField(
            model_name='truck',
            name='trucky',
            field=models.IntegerField(default=0),
        ),
    ]
