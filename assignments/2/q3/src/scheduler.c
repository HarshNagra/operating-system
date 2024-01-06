#include "../include/scheduler.h"

void *simpleRobotRoutine(void *arg) {
  Robot robot = (Robot)arg;
  Task task = robot->task;
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] starts...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
#endif 
  int jobID;
  while (!queueIsEmpty(task->jobQ)) {
    queueDequeueFront(task->jobQ, &jobID);
    int res = 0;

    if (jobID == 6) {
      sem_getvalue(&robot->task->makingCar, &res);
      if ( res == 0 ) {
        res = sem_trywait(&robot->task->car);
        if (res == 0 ) {
          #ifdef DEBUG
              debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
                RobotTypeToChar(robot->robotType), robot->id, jobID);
              printf("JOB 6 STARTED\n");
          #endif
          sem_post(&robot->task->makingBody);
          simpleWork(jobID, robot);
          sem_wait(&robot->task->makingBody);
          #ifdef DEBUG
              debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
                RobotTypeToChar(robot->robotType), robot->id, jobID);
              printf("JOB 6 DONE\n");
          #endif

          sem_post(&robot->task->skeleton);
          sem_post(&robot->task->engine);
          sem_post(&robot->task->chassis);

          sem_post(&robot->task->body);
        } else queueEnqueue(task->jobQ, &jobID);
      } else queueEnqueue(task->jobQ, &jobID);
      
    } if (jobID == 7) {
      sem_getvalue(&robot->task->makingBody, &res);
      
      if ( res == 0 ) {
        res = sem_trywait(&robot->task->body);
        if (res == 0 ) {
          #ifdef DEBUG
            debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
              RobotTypeToChar(robot->robotType), robot->id, jobID);
            printf("JOB 7 STARTED\n");
          #endif
          sem_post(&robot->task->makingCar);
          simpleWork(jobID, robot);
          sem_wait(&robot->task->makingCar);
          #ifdef DEBUG
              debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
                RobotTypeToChar(robot->robotType), robot->id, jobID);
              printf("JOB 7 DONE\n");
          #endif

          for (int i = 0; i < 4; i++) sem_post(&robot->task->tire);
          for (int i = 0; i < 7; i++) sem_post(&robot->task->window);
          sem_post(&robot->task->battery);

          sem_post(&robot->task->car);
        } else queueEnqueue(task->jobQ, &jobID);
      } else queueEnqueue(task->jobQ, &jobID);
      
    }
    
    if (jobID >= 0 && jobID < 3) {
      sem_getvalue(&robot->task->makingBody, &res);
      
      int save = 0;
      if (res == 1) {
        if (jobID == 0) {
          save = sem_trywait(&robot->task->skeleton);
          if (save == 0) {
            #ifdef DEBUG
              debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
                  RobotTypeToChar(robot->robotType), robot->id, jobID);
            #endif
            simpleWork(jobID, robot);
          }
          else queueEnqueue(task->jobQ, &jobID);
        } else if (jobID == 1) {
          save = sem_trywait(&robot->task->engine);
          if (save == 0) {
             #ifdef DEBUG
              debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
                  RobotTypeToChar(robot->robotType), robot->id, jobID);
            #endif
            simpleWork(jobID, robot);
          }
          else queueEnqueue(task->jobQ, &jobID);
        } else {
          save = sem_trywait(&robot->task->chassis);
          if (save == 0) {
            #ifdef DEBUG
              debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
                  RobotTypeToChar(robot->robotType), robot->id, jobID);
            #endif            
            simpleWork(jobID, robot);
          }
          else queueEnqueue(task->jobQ, &jobID);
        }
      } else queueEnqueue(task->jobQ, &jobID);
    } else if (jobID >= 3 && jobID < 6) {
      sem_getvalue(&robot->task->makingCar, &res);
      int save = 0;
      if (res == 1) {
        if (jobID == 3) {
          save = sem_trywait(&robot->task->battery);
          if (save == 0) {
            #ifdef DEBUG
              debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
                  RobotTypeToChar(robot->robotType), robot->id, jobID);
            #endif            
            simpleWork(jobID, robot);
          }
          else queueEnqueue(task->jobQ, &jobID);
        } else if (jobID == 4) {
          save = sem_trywait(&robot->task->window);
          if (save == 0) {
            #ifdef DEBUG
              debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
                  RobotTypeToChar(robot->robotType), robot->id, jobID);
            #endif            
            simpleWork(jobID, robot);
          }
          else queueEnqueue(task->jobQ, &jobID);
        } else {
          save = sem_trywait(&robot->task->tire);
          if (save == 0) {
            #ifdef DEBUG
              debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
                  RobotTypeToChar(robot->robotType), robot->id, jobID);
            #endif            
            simpleWork(jobID, robot);
          }
          else queueEnqueue(task->jobQ, &jobID);
        }
      } else queueEnqueue(task->jobQ, &jobID);
      
    }
  }
  pthread_exit(NULL);
}

void simpleWork(int jobID, Robot robot) {
  double timer = getCurrentTime();
  switch (jobID) {
  case SKELETON:
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] making skeleton...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
    fflush(stdout);
#endif
    makeSkeleton(robot);
    break;
  case ENGINE:
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] making engine...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
    fflush(stdout);
#endif
    makeEngine(robot);
    break;
  case CHASSIS:
#ifdef DEBUG
    debug_printf(__func__, "Robot%c[%d] making chassis...\n", 
        RobotTypeToChar(robot->robotType), robot->id);
    fflush(stdout);
#endif
    makeChassis(robot);
    break;
  case BODY:
    makeBody(robot);
    break;
  case WINDOW:
    makeWindow(robot);
    break;
  case TIRE:
    makeTire(robot);
    break;
  case BATTERY:
    makeBattery(robot);
    break;
  case CAR:
    makeCar(robot);
    break;
  default:
    err_printf(__func__, __LINE__, "Error!! Robot%c[%d] gets invalid jobID %d\n", 
        RobotTypeToChar(robot->robotType), robot->id, jobID);
    break;
  }
  timer = getCurrentTime() - timer;
#ifdef DEBUG
  debug_printf(__func__, "Robot%c[%d] job %d done! Time: %lf\n", 
      RobotTypeToChar(robot->robotType), robot->id, jobID, timer);
#endif
}


// queueDequeueFront(task->jobQ, &jobID);

//    int num = 0;
//     if (jobID <= 5) {
//       if (jobID <= 2) {
//         sem_getvalue(&body, &num);
//         if ( num > 0 ) {
          
//           #ifdef DEBUG
//               debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                   RobotTypeToChar(robot->robotType), robot->id, jobID);
//           #endif
//           simpleWork(jobID, robot);
//         } else {
//           queueEnqueue(task->jobQ, &jobID);
//           continue;
//         }
//       } else {
//         sem_getvalue(&car, &num);
//         if ( num > 0 ) {
//           #ifdef DEBUG
//               debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                   RobotTypeToChar(robot->robotType), robot->id, jobID);
//           #endif
//           simpleWork(jobID, robot);
//         } else {
//           queueEnqueue(task->jobQ, &jobID);
//           continue;
//         }
//       }
//     } else {
//       // #ifdef DEBUG
//       //     int bod = sem_getvalue(&task->makingBody, &bod);
//       //     int car = sem_getvalue(&task->makingCar, &car);
//       //     printf("Job 6 & 7: %d %d", bod, car);
//       // #endif
//       int freeRobots = 0;
//       if (jobID == 6) {
//         #ifdef DEBUG
//               debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                 RobotTypeToChar(robot->robotType), robot->id, jobID);
//         #endif
//         sem_post(&body);
//         #ifdef DEBUG
//           int bod = 0;
//           sem_getvalue(&body, &bod);
//           printf("Job 6: %d \n", bod);
//         #endif
//         simpleWork(jobID, robot);
//         sem_wait(&body);
//       } else { // jobID = 7
//         #ifdef DEBUG
//               debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                 RobotTypeToChar(robot->robotType), robot->id, jobID);
//         #endif
//         sem_post(&car);
//         #ifdef DEBUG
//             int c = 0;
//             sem_getvalue(&car, &c);
//             printf("Job 7: %d \n", c);
//         #endif
//         simpleWork(jobID, robot);
//         sem_wait(&car);
//       }
//       // while ( !(freeRobots > 0) ) {
//       //   freeRobots = getNumFreeRobot(TypeA) + getNumFreeRobot(TypeB) + getNumFreeRobot(TypeC);
//       //   if ( freeRobots > 1 ) {
          
//       //     }
//       // }
//     }
//   }



// void *simpleRobotRoutine(void *arg) {
//   Robot robot = (Robot)arg;
//   Task task = robot->task;
//   sem_t body = task->makingBody;
//   sem_t car = task->makingCar;
// #ifdef DEBUG
//     debug_printf(__func__, "Robot%c[%d] starts...\n", 
//         RobotTypeToChar(robot->robotType), robot->id);
// #endif 
//   int jobID;
//   while (!queueIsEmpty(task->jobQ)) {
//     queueDequeueFront(task->jobQ, &jobID);
//     if (jobID <= 5) {
//       if (jobID <= 2) {
//         int res = sem_trywait(&robot->task->makingBody);
//         if ( res == 0 ) {
//           #ifdef DEBUG
//             debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                 RobotTypeToChar(robot->robotType), robot->id, jobID);
//           #endif
//           simpleWork(jobID, robot);
//         }
//         else queueEnqueue(task->jobQ, &jobID);
//       } else {
//         int res = sem_trywait(&robot->task->makingCar);
//         if (res == 0) {
//           #ifdef DEBUG
//             debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                 RobotTypeToChar(robot->robotType), robot->id, jobID);
//           #endif
//           simpleWork(jobID, robot);
//         }
//         else queueEnqueue(task->jobQ, &jobID);
//       }
//     } else {
//         int robots = 0;
//         sem_getvalue(&robot->task->robots, &robots);
//         int checkBody = 0;
//         sem_getvalue(&robot->task->makingBody, &checkBody);
//         int checkCar = 0;
//         sem_getvalue(&robot->task->makingBody, &checkCar);
//         if ((getNumFreeSpace() > 1 && robots > 1) || (robots > 1 && checkBody == 0 && getNumProducedBody() > 0 && getNumFreeSpace() > 0 && jobID == 7)) {
//           sem_wait(&robot->task->robots);
//           if (jobID == 6) {
//           #ifdef DEBUG
//                 debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                   RobotTypeToChar(robot->robotType), robot->id, jobID);
//           #endif
//           for ( int i=0; i<3; i++) sem_post(&robot->task->makingBody);
//           #ifdef DEBUG
//             int bod = 0;
//             sem_getvalue(&robot->task->makingBody, &bod);
//             printf("Job 6: %d \n", bod);
//           #endif
//           simpleWork(jobID, robot);
//         } else { // jobID = 7
//           #ifdef DEBUG
//                 debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                   RobotTypeToChar(robot->robotType), robot->id, jobID);
//           #endif
//           for (int i = 0; i < 12; i++) sem_post(&robot->task->makingCar);
//           #ifdef DEBUG
//               int c = 0;
//               sem_getvalue(&robot->task->makingCar, &c);
//               printf("Job 7: %d \n", c);
//           #endif
//           simpleWork(jobID, robot);
//         }
//         sem_post(&robot->task->robots);
//       } else {
//         queueEnqueue(task->jobQ, &jobID);
//       }
      
//     }
//   }
//   pthread_exit(NULL);
// }

//latest

// while (!queueIsEmpty(task->jobQ)) {
//     int res = -1;
//     queueDequeueFront(task->jobQ, &jobID);
//     if (jobID <= 5) {
//       if (jobID <= 2) {
//         sem_getvalue(&robot->task->makingBody, &res);
//         if ( res == 1 ) {
//           #ifdef DEBUG
//             debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                 RobotTypeToChar(robot->robotType), robot->id, jobID);
//           #endif
//           simpleWork(jobID, robot);
//         }
//         else queueEnqueue(task->jobQ, &jobID);
//       } else {
//         sem_getvalue(&robot->task->makingCar, &res);
//         if (res == 1) {
//           #ifdef DEBUG
//             debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                 RobotTypeToChar(robot->robotType), robot->id, jobID);
//           #endif
//           simpleWork(jobID, robot);
//         }
//         else queueEnqueue(task->jobQ, &jobID);
//       }
//     } else {
//       int robots = 0;
//       sem_getvalue(&robot->task->robots, &robots);
//       if (jobID == 6) {
//         sem_getvalue(&robot->task->makingBody, &res);
//         if ((res == 0 && getNumFreeSpace() > 1 && robots > 1 &&  getNumProducedBody() == 0) ) {
//           // || (res == 1 && getNumFreeSpace() > 1 && robots > 3)
//           sem_wait(&robot->task->robots);
//           #ifdef DEBUG
//                 debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                   RobotTypeToChar(robot->robotType), robot->id, jobID);
//           #endif
//           sem_post(&robot->task->makingBody);
//           #ifdef DEBUG
//             int bod = 0;
//             sem_getvalue(&robot->task->makingBody, &bod);
//             printf("Job 6 START \n");
//           #endif
//           simpleWork(jobID, robot);
//           #ifdef DEBUG
//             printf("Job 6 DONE \n");
//           #endif
//           sem_wait(&robot->task->makingBody);
//           sem_post(&robot->task->robots);
//         } else queueEnqueue(task->jobQ, &jobID);
//       } else {
//         sem_getvalue(&robot->task->makingCar, &res);
//         if ((res == 0 && getNumFreeSpace() > 1 && robots > 1 || (res == 0 && getNumProducedBody() == 1 && robots > 1 && getNumFreeSpace() == 1)) ) {
//           //|| (getNumProducedBody() == 1 && robots > 1 && getNumFreeSpace() > 0)
//           sem_wait(&robot->task->robots);
//           #ifdef DEBUG
//                 debug_printf(__func__, "Robot%c[%d]: working on job %d...\n", 
//                   RobotTypeToChar(robot->robotType), robot->id, jobID);
//           #endif
//           sem_post(&robot->task->makingCar);
//           #ifdef DEBUG
//               printf("Job 7 START \n");
//           #endif
//           simpleWork(jobID, robot);
//           #ifdef DEBUG
//               printf("Job 7 END \n");
//           #endif
//           sem_wait(&robot->task->makingCar);

//           sem_post(&robot->task->robots);
//         } else queueEnqueue(task->jobQ, &jobID);
//       }
//     }
//   }