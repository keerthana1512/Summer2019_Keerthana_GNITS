import openpyxl
import click
import os
from bs4 import BeautifulSoup

os.environ.setdefault('DJANGO_SETTINGS_MODULE','classproject.settings')
import django
django.setup();
from onlineapp.models import *

def get_data(sheet):
    data=[]
    for row in sheet.iter_rows():
        rowSelected = []
        for cell in row:
            rowSelected.append(cell.value)
        data.append(rowSelected)
    return data

def extractDBname(string):
    return string.split('_')[2]


@click.group()
@click.option('-args')
def cli(args):
    pass

@cli.command(help="import data from excel to database using Django models")
#@click.argument("students_excel")
#@click.argument("mock_result_excel")
def importdata():

    wb = openpyxl.load_workbook("students.xlsx")
    ws = wb.get_sheet_by_name('Colleges')
    college_data=get_data(ws)[1:]
    for i in range(len(college_data)):
        c = College(name=college_data[i][0], location=college_data[i][2], acronym=college_data[i][1], contact=college_data[i][3])
        c.save()

    ws = wb.get_sheet_by_name('Current')
    student_data = get_data(ws)[1:]
    for i in range(len(student_data)):
            s = Student(name=student_data[i][0], college=College.objects.get(acronym=student_data[i][1]), email=student_data[i][2], db_folder=student_data[i][3],dropped_out=0)
            s.save()

    ws = wb.get_sheet_by_name('Deletions')
    student_data = get_data(ws)[1:]
    for i in range(len(student_data)):
        s = Student(name=student_data[i][0], college=College.objects.get(acronym=student_data[i][1]),
                    email=student_data[i][2], db_folder=student_data[i][3].lower(), dropped_out=1)
        print(s)
        s.save()

    soup = BeautifulSoup(open("mock_results.html"), "html.parser")
    table = soup.find('table')
    table_rows = table.find_all('tr')
    for i in range(1, len(table_rows)):
        td = table_rows[i].find_all('td')
        row = [td[i].text for i in range(1, len(td))]
        try:
            m = MockTest1(problem1=int(row[1]), problem2=int(row[2]), problem3=int(row[3]), problem4=int(row[4]),
                          total=int(row[5]),
                          student=Student.objects.get(db_folder=extractDBname(row[0])))
            m.save()
        except Exception as e:
            print(e)


if __name__=='__main__':
    cli()
