// This file is part of RSS Guard.
//
// Copyright (C) 2011-2015 by Martin Rotter <rotter.martinos@gmail.com>
//
// RSS Guard is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RSS Guard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RSS Guard. If not, see <http://www.gnu.org/licenses/>.

/******************************************************************************
Copyright (c) 2010, Artem Galichkin <doomer3d@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include "dynamic-shortcuts/shortcutcatcher.h"

#include "dynamic-shortcuts/shortcutbutton.h"
#include "gui/plaintoolbutton.h"
#include "miscellaneous/iconfactory.h"

#include <QHBoxLayout>


ShortcutCatcher::ShortcutCatcher(QWidget *parent)
  : QWidget(parent) {
  // Setup layout of the control
  m_layout = new QHBoxLayout(this);
  m_layout->setMargin(0);
  m_layout->setSpacing(1);

  // Create reset button.
  m_btnReset = new PlainToolButton(this);
  m_btnReset->setIcon(qApp->icons()->fromTheme(QSL("edit-revert")));
  m_btnReset->setFocusPolicy(Qt::NoFocus);
  m_btnReset->setToolTip(tr("Reset to original shortcut."));

  // Create clear button.
  m_btnClear = new PlainToolButton(this);
  m_btnClear->setIcon(qApp->icons()->fromTheme(QSL("item-remove")));
  m_btnClear->setFocusPolicy(Qt::NoFocus);
  m_btnClear->setToolTip(tr("Clear current shortcut."));

  // Clear main shortcut catching button.
  m_btnChange = new ShortcutButton(this);
  m_btnChange->setFocusPolicy(Qt::StrongFocus);
  m_btnChange->setToolTip(tr("Click and hit new shortcut."));

  // Add both buttons to the layout.
  m_layout->addWidget(m_btnChange);
  m_layout->addWidget(m_btnReset);
  m_layout->addWidget(m_btnClear);

  // Establish needed connections.
  connect(m_btnReset, SIGNAL(clicked()), this, SLOT(resetShortcut()));
  connect(m_btnClear, SIGNAL(clicked()), this, SLOT(clearShortcut()));
  connect(m_btnChange, SIGNAL(clicked()), this, SLOT(startRecording()));

  // Prepare initial state of the control.
  updateDisplayShortcut();
}

ShortcutCatcher::~ShortcutCatcher() {
  delete m_btnReset;
  delete m_btnChange;
  delete m_btnClear;
  delete m_layout;
}

void ShortcutCatcher::startRecording() {
  m_numKey = 0;
  m_modifierKeys = 0;
  m_currentSequence = QKeySequence();
  m_isRecording = true;
  m_btnChange->setDown(true);
  m_btnChange->grabKeyboard();

  updateDisplayShortcut();
}

void ShortcutCatcher::doneRecording() {
  m_isRecording = false;
  m_btnChange->releaseKeyboard();
  m_btnChange->setDown(false);

  updateDisplayShortcut();

  emit shortcutChanged(m_currentSequence);
}

void ShortcutCatcher::controlModifierlessTimout() {
  if (m_numKey && !m_modifierKeys) {
    doneRecording();
  }
}

void ShortcutCatcher::updateDisplayShortcut() {
  QString str = m_currentSequence.toString(QKeySequence::NativeText);
  str.replace(QL1S("&"), QL1S("&&"));

  if (m_isRecording) {
    if (m_modifierKeys) {
      if (!str.isEmpty()) {
        str.append(QSL(","));
      }
      if (m_modifierKeys & Qt::META) {
        str += QL1S("Meta + ");
      }
      if (m_modifierKeys & Qt::CTRL) {
        str += QL1S("Ctrl + ");
      }
      if (m_modifierKeys & Qt::ALT) {
        str += QL1S("Alt + ");
      }
      if (m_modifierKeys & Qt::SHIFT) {
        str += QL1S("Shift + ");
      }
    }
  }

  m_btnChange->setText(str);
}
