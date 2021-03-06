#include "controller.h"

#include <QAction>
#include <QActionGroup>
#include <QString>
#include <QMessageBox>

Controller::Controller() : model_(std::make_unique<Model>(this)),
    view_(std::make_unique<MainWindow>(this, model_.get())) {
  ConnectMainPageButtons();
  ConnectPickAnOptionButtons();
  ConnectInputAnswerButtons();
  ConnectAudioButtons();

  view_->show();
  view_->Update();
}

void Controller::ManageDifficultyMenu(QMenu* difficulty_menu) {
  auto* set_easy_difficulty = difficulty_menu->addAction("Легкая");
  auto* set_medium_difficulty = difficulty_menu->addAction("Средняя");
  auto* set_hard_difficulty = difficulty_menu->addAction("Сложная");

  set_easy_difficulty->setCheckable(true);
  set_medium_difficulty->setCheckable(true);
  set_hard_difficulty->setCheckable(true);

  auto* difficulty_group = new QActionGroup(this);
  difficulty_group->addAction(set_easy_difficulty);
  difficulty_group->addAction(set_medium_difficulty);
  difficulty_group->addAction(set_hard_difficulty);
  difficulty_group->setExclusive(true);

  connect(set_easy_difficulty, &QAction::triggered, this,
          [this] {
            model_->SetDifficultyMode("easy");
            view_->statusBar()->showMessage("Легче - не значит проще)");
          });
  connect(set_medium_difficulty, &QAction::triggered, this,
          [this] {
            model_->SetDifficultyMode("medium");
            view_->statusBar()->showMessage("Не определился?)");
          });
  connect(set_hard_difficulty, &QAction::triggered, this,
          [this] {
            model_->SetDifficultyMode("hard");
            view_->statusBar()->showMessage("Не darksouls конечно, но..)");
          });

  if (model_->GetDifficultyMode() == "easy") {
    set_easy_difficulty->setChecked(true);
  } else if (model_->GetDifficultyMode() == "medium") {
    set_medium_difficulty->setChecked(true);
  } else {  // hard
    set_hard_difficulty->setChecked(true);
  }
}

void Controller::ManageSoundMenu(QMenu* sound_menu) {
  auto* set_sound_on = sound_menu->addAction("Включить");
  auto* set_sound_off = sound_menu->addAction("Выключить");

  set_sound_on->setCheckable(true);
  set_sound_off->setCheckable(true);

  auto* sound_group = new QActionGroup(this);
  sound_group->addAction(set_sound_on);
  sound_group->addAction(set_sound_off);
  sound_group->setExclusive(true);

  connect(set_sound_on, &QAction::triggered, this,
          [this] {
            model_->SetSoundMode("on");
            view_->statusBar()->showMessage("by Hans Zimmer)");
            view_->Update();
          });
  connect(set_sound_off, &QAction::triggered, this,
          [this] {
            model_->SetSoundMode("off");
            view_->statusBar()->showMessage("Тишина должна быть в библиотеке)");
            view_->Update();
          });

  if (model_->IsSoundOn()) {
    set_sound_on->setChecked(true);
  } else {
    set_sound_off->setChecked(true);
  }
}

void Controller::ConnectResetProgressAction(QAction* reset_progress_action) {
  connect(reset_progress_action, &QAction::triggered, this,
          &Controller::ShowResetProgressDialog);
}

void Controller::ConnectExitButton(QPushButton* exit_button) {
  connect(exit_button, &QPushButton::pressed, this,
          &Controller::ShowExitDialog);
  exit_button->setShortcut(QKeySequence("Ctrl+Q"));
}

void Controller::ShowResetProgressDialog() {
  auto answer = QMessageBox::question(view_->centralWidget(), "Duolingo",
                                      "Вы уверены, что хотите сбросить текущий прогресс?");
  if (answer == QMessageBox::Yes) {
    model_->ResetProgressPoints();
    view_->statusBar()->showMessage("Начнем с презент симпл)");
    view_->Update();
  } else {
    view_->statusBar()->showMessage("Это было опасно)");
  }
}

void Controller::ShowExitDialog() {
  auto answer = QMessageBox::question(view_->centralWidget(), "Duolingo",
                                      "Вы уверены, что хотите выйти?");
  if (answer == QMessageBox::Yes) {
    view_->close();
  } else {
    view_->statusBar()->showMessage("Не выходи из комнаты, не совершай ошибку)");
  }
}

void Controller::ConnectMainPageButtons() {
  auto* widget = view_->GetMainPage();
  connect(widget, &MainPageWidget::PickAnOptionButtonPressed, this, [this] {
    model_->StartNewExercise("pick_an_option");
    view_->GoToPickAnOption();
    auto widget = view_->GetPickAnOption();
    widget->SetTaskCondition(model_->GetCondition());
    widget->SetVariants(model_->GetVariants());
    widget->UpdateForNextTask();
  });
  connect(widget, &MainPageWidget::InputAnswerButtonPressed, this, [this] {
    model_->StartNewExercise("input_answer");
    view_->GoToInputAnswer();
    auto widget = view_->GetInputAnswer();
    widget->SetTaskCondition(model_->GetCondition());
    widget->UpdateForNextTask();
  });
  connect(widget, &MainPageWidget::AudioButtonPressed, this, [this] {
    model_->StartNewExercise("audio");
    view_->GoToAudio();
    auto widget = view_->GetAudio();
    widget->SetTaskCondition(model_->GetCondition());
    widget->SetVariants(model_->GetVariants());
    widget->UpdateForNextTask();
  });
  connect(widget, &MainPageWidget::MixedButtonPressed, this, [=] {
    int index = rand() % 3;
    switch (index) {
      case 0: {
        widget->PickAnOptionButtonPressed();
        break;
      }
      case 1: {
        widget->InputAnswerButtonPressed();
        break;
      }
      case 2: {
        widget->AudioButtonPressed();
        break;
      }
    }
  });
}

void Controller::ConnectPickAnOptionButtons() {
  auto* widget = view_->GetPickAnOption();
  connect(widget, &PickAnOptionWidget::CheckAnswerButtonPressed, this, [=] {
    widget->UpdateAfterCheck();
    model_->CheckAnswer(widget->GetAnswer());
    view_->Update();
  });
  connect(widget, &PickAnOptionWidget::NextQuestionButtonPressed, this, [=] {
    model_->SwitchToNextTask();
    widget->SetTaskCondition(model_->GetCondition());
    widget->SetVariants(model_->GetVariants());
    widget->UpdateForNextTask();
  });
  connect(widget, &PickAnOptionWidget::GoToMainPageButtonPressed, this, [this] {
    view_->GoToMainPage();
  });
}

void Controller::ConnectInputAnswerButtons() {
  auto* widget = view_->GetInputAnswer();
  connect(widget, &InputAnswerWidget::CheckAnswerButtonPressed, this, [=] {
    widget->UpdateAfterCheck();
    model_->CheckAnswer(widget->GetAnswer());
    view_->Update();
  });
  connect(widget, &InputAnswerWidget::NextQuestionButtonPressed, this, [=] {
    model_->SwitchToNextTask();
    widget->SetTaskCondition(model_->GetCondition());
    widget->UpdateForNextTask();
  });
  connect(widget, &InputAnswerWidget::GoToMainPageButtonPressed, this, [this] {
    view_->GoToMainPage();
  });
}

void Controller::ConnectAudioButtons() {
  auto* widget = view_->GetAudio();
  connect(widget, &AudioWidget::CheckAnswerButtonPressed, this, [=] {
    widget->UpdateAfterCheck();
    model_->CheckAnswer(widget->GetAnswer());
    view_->Update();
  });
  connect(widget, &AudioWidget::NextQuestionButtonPressed, this, [=] {
    model_->SwitchToNextTask();
    widget->SetTaskCondition(model_->GetCondition());
    widget->SetVariants(model_->GetVariants());
    widget->UpdateForNextTask();
  });
  connect(widget, &AudioWidget::GoToMainPageButtonPressed, this, [this] {
    view_->GoToMainPage();
  });
}

void Controller::OnCorrect() {
  view_->OnCorrect();
  view_->statusBar()->showMessage("Ваш ответ правильный");
}

void Controller::OnWrong() {
  view_->OnWrong();
  view_->statusBar()->showMessage("Правильный ответ: " + model_->GetCorrectAnswer());
}

void Controller::OnWin() {
  view_->OnWin();
  view_->statusBar()->showMessage(
      "Победа победа куриный ужин) Твой ответ правильный");

}

void Controller::OnLose() {
  view_->OnLose();
  view_->statusBar()->showMessage(
      "Надеюсь ты хоть не у Ситниковой) Правильный ответ: "
      + model_->GetCorrectAnswer());
}
