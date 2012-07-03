/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/
#include <map>
using namespace std;
//! [0]
#include <QtUiTools>
//! [0]
#include <QtGui>

#include "linehandlingform.h"

//! [1]
LineHandlingForm::LineHandlingForm(QWidget *parent)
    : QWidget(parent)
{
    QUiLoader loader;

    QFile file(":/forms/linehandlingform.ui");
    file.open(QFile::ReadOnly);
    QWidget *formWidget = loader.load(&file, this);
    file.close();
//! [1]

//! [2]
   /* ui_inputSpinBox1 = findChild<QSpinBox*>("inputSpinBox1");
    ui_inputSpinBox2 = findChild<QSpinBox*>("inputSpinBox2");
    ui_outputWidget = findChild<QLabel*>("outputWidget");*/
	inputReg = findChild<QTextEdit*>("inputReg");
	outputReg = findChild<QTextEdit*>("outputReg");
	inputFilename = findChild<QLineEdit*>("inputFilename");
	outputFilename = findChild<QLineEdit*>("outputFilename");
//! [2]
	inputFilename->setText("test.txt");
	outputFilename->setText("out.txt");
	inputReg->setText("(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)\\s+(\\S+)\\s(\\S+)\\s(\\S+)\\s(\\S+)\\s(\\S+)\\s(\\S+)");
	outputReg->setText("($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11)");
//! [3]
    QMetaObject::connectSlotsByName(this);
//! [3]

//! [4]
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(formWidget);
    setLayout(layout);

    setWindowTitle(tr("Calculator Builder"));
}
//! [4]
/*
//! [5]
void LineHandlingForm::on_inputSpinBox1_valueChanged(int value)
{
    ui_outputWidget->setText(QString::number(value + ui_inputSpinBox2->value()));
}
//! [5] //! [6]

//! [6] //! [7]
void LineHandlingForm::on_inputSpinBox2_valueChanged(int value)
{
    ui_outputWidget->setText(QString::number(value + ui_inputSpinBox1->value()));
}
//! [7]*/

map<QString,QString> makeReg(QString value,QString patten)
{
	map<QString,QString> result;
	QRegExp rxlen(patten);
	int pos = rxlen.indexIn(value);
	if (pos > -1) {

		for(int i=1,count=rxlen.captureCount(); i<=count;i++)
		{
			QString value = rxlen.cap(i); 
			result["$"+QString::number(i)]=value;
		}
	}
	return result;
}
QString transformObject(QString &patten,map<QString,QString> & values,int &i)
{
	QString value_name="$";
	if(patten[i]==QChar('$'))
		return QString(patten[i++]);
	for(;i<patten.length() && (patten[i].isLetter()||patten[i].isDigit()); i++){
		value_name+=patten[i];
	}
	if(patten[i]==QChar(';')){
		i++;
	}
	if(values.find(value_name)!=values.end()){
		return values[value_name];
	}
	return "";//QString("{")+"name:"+value_name+":"+values[value_name]+","+QString::number(values.size())+"}";
}
QString transformString(QString patten,map<QString,QString> &values)
{
	QString res;
	for(int i=0; i<patten.length(); )
	{
		if(patten[i]=='$')
		{
			i++;
			res+=transformObject(patten,values,i);
			//res+="("+QString::number(i)+")";
		}else{
			res+=patten[i];//+"("+QString::number(patten[i]==QChar('$'))+","+QString::number(i)+")";
			i++;
		}
	}
	return res;
}
void LineHandlingForm::on_buSolve_clicked()
{
    //ui_outputWidget->setText(QString::number(33));
	QString filename=inputFilename->text();
	QString output_filename=outputFilename->text();
	QString input_reg=inputReg->toPlainText();
	QString output_reg=outputReg->toPlainText();
	
	QFile fin(filename),fout(output_filename);
	if (!fin.open(QIODevice::ReadOnly ))//| QIODevice::Text
         return;
	if (!fout.open(QIODevice::WriteOnly))// | QIODevice::Text
         return;
	QTextStream out(&fout);
	//QString res;
	while(!fin.atEnd()){
		QString data=fin.readLine();
		map<QString,QString> values=makeReg(data,input_reg);
		out<<transformString(output_reg,values);
		//res+=transformString(output_reg,values)+"\n";
	}
	
	

	/*for(map<QString,QString>::iterator it=values.begin(),end=values.end(); it!=end; it++)
	{
		res+=it->first+":"+it->second+"\n";
	}*/
	//outputFilename->setText("---"+res);
}
