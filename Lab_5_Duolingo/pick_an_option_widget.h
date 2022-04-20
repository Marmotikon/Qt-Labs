#pragma once

#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class PickAnOptionWidget : public QWidget {
  Q_OBJECT
 public:
  PickAnOptionWidget();

  void SetTaskCondition(QString string);
  void SetTaskText(QString string);
  void SetVariants(int count, std::vector<QString> strings);

 signals:
  void CheckAnswerButtonPressed();
  void NextQuestionButtonPressed();
  void GoToMainPageButtonPressed();

 private:
  QGridLayout* layout_;
  QLabel* task_condition_;
  QLabel* task_text_;
  QButtonGroup* variants_;
  QPushButton* check_answer_button_;
  QPushButton* next_question_button_;
  QPushButton* go_to_main_page_button_;
};
