from django.urls import path

from . import views
from .views import *
urlpatterns = [
    path('', views.home, name='home'),
    #path('status/', views.index_status, name='status'),

    path('rideowner/', views.rideowner_home, name='rideowner-home'),
    path('rideowner/request/', RideOwnerCreateView.as_view(), name='rideowner-request'),
    path('rideowner/status/', RideOwnerListView.as_view(), name='rideowner-status'),
    path('rideowner/status/<int:pk>/update/', RideOwnerUpdateView.as_view(), name='rideowner-update'),
    path('rideowner/status/<int:pk>/delete/', RideOwnerDeleteView.as_view(), name='rideowner-delete'),

    #path of become a driver
    
    path('ridedriver/', views.ridedriver_home, name='ridedriver-home'),
    path('ridedriver/ridesearching/', RideDriverSearchView.as_view(), name='ridedriver-search'),
    #worklistview + pastlistview
    path('ridedriver/status/', RideDriverStatusView.as_view(), name='ridedriver-status'),
    path('ridedriver/<int:rid>/confirm/', views.confirm, name='ridedriver-confirm'),
    path('ridedriver/<int:rid>/complete/', views.complete, name='ridedriver-complete'),
    
    path('ridesharer/', views.ridesharer_home, name='ridesharer-home'),
    path('ridesharer/ridesharing/', RideSharerSearchView.as_view(), name='ridesharer-search'),
    path('ridesharer/searchresult', RideSharerResultView.as_view(), name='ridesharer-result'),
    path('ridesharer/status/', RideSharerStatusView.as_view(), name='ridesharer-status'),
    path('ridesharer/<int:rid>/join/', views.join, name='ridesharer-join'),
    path('ridesharer/<int:rid>/cancel/', views.cancel, name='ridesharer-cancel'),
]
