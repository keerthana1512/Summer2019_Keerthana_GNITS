from django.db import models

# Create your models here.

class Todoitem(models.Model):
    descrioption = models.CharField(max_length=300)
    due_date=models.DateField(null=True)
    completed=models.BooleanField


    def __str__(self):
        return self.descrioption


class Todolist(models.Model):
    name = models.CharField(max_length=128)
    created = models.DateField(auto_now=True)

    def __str__(self):
        return self.name

