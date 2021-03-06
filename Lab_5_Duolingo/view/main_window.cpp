#include "main_window.h"

#include <QMessageBox>
#include <QActionGroup>

#include <iostream>

#include "input_answer_widget.h"
#include "pick_an_option_widget.h"

MainWindow::MainWindow(AbstractController* controller, Model* model) :
    QMainWindow(nullptr), //todo
    controller_(controller),
    model_(model),
    progress_points_(new QLabel("Набранные очки: ...")),
    progress_bar_(new QProgressBar(this)),

    central_widget_(new QWidget(this)),
    central_layout_(new QVBoxLayout()),

    main_widget_(new QStackedWidget()),
    main_page_widget_(new MainPageWidget()),
    pick_an_option_widget_(new PickAnOptionWidget()),
    input_answer_widget_(new InputAnswerWidget()),
    audio_widget_(new AudioWidget()),

    attempts_(new QLabel("Оставшиеся попытки: ...")),
    exit_(new QPushButton("Выход из игры")),

    right_sound_(new QSoundEffect(this)),
    wrong_sound_(new QSoundEffect(this)),
    lose_sound_(new QSoundEffect(this)),
    win_sound_(new QSoundEffect(this)) {

  resize(500, 500);
  setMinimumSize(300, 300);
  setWindowTitle("Duolingo (made in China)");
  setWindowIcon(QIcon(":/images/icon.jpg"));

  statusBar()->setVisible(true);
  CreateMenu();
  ManageCentralWidget();
  ManageSounds();
  GoToMainPage();

  controller_->ConnectExitButton(exit_);
}

void MainWindow::ManageCentralWidget() {
  central_layout_->addWidget(progress_points_);

  main_widget_->addWidget(main_page_widget_);
  main_widget_->addWidget(pick_an_option_widget_);
  main_widget_->addWidget(input_answer_widget_);
  main_widget_->addWidget(audio_widget_);
  central_layout_->addWidget(main_widget_);

  central_layout_->addWidget(progress_bar_);
  central_layout_->addWidget(attempts_);
  central_layout_->addWidget(exit_);

  central_layout_->setStretch(0, 1);
  central_layout_->setStretch(1, 10);
  central_layout_->setStretch(2, 10);
  central_layout_->setStretch(3, 10);
  central_layout_->setStretch(4, 10);
  central_layout_->setStretch(5, 2);
  central_layout_->setStretch(6, 1);
  central_layout_->setStretch(7, 1);

  setCentralWidget(central_widget_);
  central_widget_->setLayout(central_layout_);
}

void MainWindow::ManageSounds() {
  right_sound_->setSource(QUrl("qrc:/sounds/answer_is_right.wav"));
  right_sound_->setLoopCount(1);
  right_sound_->setVolume(1.0f);

  wrong_sound_->setSource(QUrl("qrc:/sounds/answer_is_wrong.wav"));
  wrong_sound_->setLoopCount(1);
  wrong_sound_->setVolume(1.0f);

  lose_sound_->setSource(QUrl("qrc:/sounds/lose.wav"));
  lose_sound_->setLoopCount(1);
  lose_sound_->setVolume(1.0f);

  win_sound_->setSource(QUrl("qrc:/sounds/win.wav"));
  win_sound_->setLoopCount(1);
  win_sound_->setVolume(1.0f);
}

void MainWindow::CreateMenu() {
  auto* difficulty_menu = menuBar()->addMenu("Сложность");
  controller_->ManageDifficultyMenu(difficulty_menu);

  auto* sound_menu = menuBar()->addMenu("Звук");
  controller_->ManageSoundMenu(sound_menu);

  auto* reset_progress_action = menuBar()->addAction("Сбросить прогресс");
  controller_->ConnectResetProgressAction(reset_progress_action);

  progress_points_->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(progress_points_, &QMainWindow::customContextMenuRequested,
          reset_progress_action, &QAction::trigger);
}

void MainWindow::GoToMainPage() {
  menuBar()->show();
  main_widget_->setCurrentWidget(main_page_widget_);
  progress_bar_->hide();
  attempts_->hide();
}

void MainWindow::GoToPickAnOption() {
  GoToTaskMode();
  main_widget_->setCurrentWidget(pick_an_option_widget_);

}

void MainWindow::GoToInputAnswer() {
  GoToTaskMode();
  main_widget_->setCurrentWidget(input_answer_widget_);
}

void MainWindow::GoToAudio() {
  GoToTaskMode();
  main_widget_->setCurrentWidget(audio_widget_);
}

void MainWindow::GoToTaskMode() {
  menuBar()->hide();
  progress_bar_->show();
  progress_bar_->setValue(0);
  progress_bar_->setMaximum(model_->GetCorrectNeeded());
  attempts_->show();
  Update();
}

MainPageWidget* MainWindow::GetMainPage() {
  return main_page_widget_;
}

PickAnOptionWidget* MainWindow::GetPickAnOption() {
  return pick_an_option_widget_;
}

InputAnswerWidget* MainWindow::GetInputAnswer() {
  return input_answer_widget_;
}

AudioWidget* MainWindow::GetAudio() {
  return audio_widget_;
}

void MainWindow::Update() {
  if (model_->IsSoundOn()) {
    UnmuteSounds();
  } else {
    MuteSounds();
  }
  attempts_->setText("Оставшиеся попытки: "
      + QString::number(model_->GetAttemptsRemained()));
  progress_points_->setText("Текущий прогресс: "
      + QString::number(model_->GetProgressPoints()));
  progress_bar_->setValue(model_->GetCurrentCorrectCount());
}

void MainWindow::MuteSounds() {
  right_sound_->setMuted(true);
  wrong_sound_->setMuted(true);
  lose_sound_->setMuted(true);
  win_sound_->setMuted(true);
}

void MainWindow::UnmuteSounds() {
  right_sound_->setMuted(false);
  wrong_sound_->setMuted(false);
  lose_sound_->setMuted(false);
  win_sound_->setMuted(false);
}

void MainWindow::OnCorrect() {
  right_sound_->play();
}

void MainWindow::OnWrong() {
  wrong_sound_->play();
}

void MainWindow::OnWin() {
  win_sound_->play();
  GoToMainPage();
  Update();
  // GoToWinPage(); todo
}

void MainWindow::OnLose() {
  lose_sound_->play();
  GoToMainPage();
  Update();
  // GoToLosePage(); todo
}

