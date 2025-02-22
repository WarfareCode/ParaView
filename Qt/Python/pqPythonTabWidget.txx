/*=========================================================================

   Program: ParaView
   Module:    pqPythonTabWidget.txx

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2.

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "pqPythonTextArea.h"

#include <QTabBar>

#include <iostream>

//-----------------------------------------------------------------------------
template <>
inline void pqPythonTabWidget::linkTo<QTextEdit>(QTextEdit* obj)
{
  const auto FindLinkedWidget = [this](const QObject* obj) -> pqPythonTextArea* {
    pqPythonTextArea* linkedWidget = nullptr;
    for (int i = 0; i < this->count() - 1; ++i)
    {
      pqPythonTextArea* widget = this->getWidget<pqPythonTextArea>(i);
      if (widget && widget->isLinkedTo(obj))
      {
        return widget;
      }
    }

    return nullptr;
  };

  pqPythonTextArea* linkedWidget = FindLinkedWidget(obj);
  if (!linkedWidget)
  {
    this->createNewEmptyTab();
    linkedWidget = this->getCurrentTextArea();

    linkedWidget->linkTo(obj);
    const QString txt = obj->toPlainText();
    if (!txt.isEmpty())
    {
      linkedWidget->setText(txt);
    }

    // We set a callback on the QTextEdit in the associated filter gets destroyed
    this->connect(obj, &QObject::destroyed, [this, FindLinkedWidget](QObject* object) {
      pqPythonTextArea* linkedWidget = FindLinkedWidget(object);
      if (linkedWidget)
      {
        linkedWidget->unlink();
        if (linkedWidget->isEmpty())
        {
          this->tabCloseRequested(this->indexOf(linkedWidget));
        }
        else
        {
          this->updateTab(linkedWidget);
        }
      }
    });
  }

  this->tabBar()->setCurrentIndex(this->indexOf(linkedWidget));
  this->updateTab(linkedWidget);
}
