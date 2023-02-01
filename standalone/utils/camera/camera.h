#include "../common/common.h"
#include "../common/matrix_operations.h"

class W3Camera {
    //public
    public:
        //Camera matrix
        Matrix4x4 cameraMatrix; 
        float movementSpeed = 0.2f; 

        // Constructor 
        W3Camera(float movementSpeed){
            this->cameraMatrix = getMatrixRotationY(0.f);
            this->movementSpeed = movementSpeed;
        }

        W3Camera(){
            this->cameraMatrix = getMatrixRotationY(0.f);
        }

        W3Camera(Vect3d position){
            this->cameraMatrix = getMatrixRotationY(0.f);
            this->setTranslation(position);
        }


        Vect3d getXDir(){
            return {this->cameraMatrix.m[0][0],this->cameraMatrix.m[0][1],this->cameraMatrix.m[0][2],1};
        };

        // Translate 
        void setTranslation(Vect3d pos);
        // Get translation
        Vect3d getTranslation();
        // Get Z Dir
        Vect3d getZDir();
        // Get the inverse of the camera matrix
        Matrix4x4 get4x4MatrixInverse();
        // Set movement speed
        void setMovementSpeed(float speed);
        // Move foward
        void moveFoward(float distance);
        void moveBack();
        // Rotate Y
        void rotateY(float angle);
        // Move up
        void moveUp(float distance);
        // Move left and right
        void moveLeft(float distance);
        void moveRight(float distance);

};


// Returns the camera translation
Vect3d W3Camera::getTranslation(){
    Vect3d res; 
    res.x = this->cameraMatrix.m[3][0];
    res.y = this->cameraMatrix.m[3][1];
    res.z = this->cameraMatrix.m[3][2];
    return res;
}

// Sets the translation for the camera
void W3Camera::setTranslation(Vect3d vect){
    this->cameraMatrix.m[3][0] = vect.x; 
    this->cameraMatrix.m[3][1] = vect.y; 
    this->cameraMatrix.m[3][2] = vect.z; 
}

// Returns the inverse of the camera matrix 
Matrix4x4 W3Camera::get4x4MatrixInverse(){
    return Matrix_QuickInverse(this->cameraMatrix);
}

// Sets the movement speed
void W3Camera::setMovementSpeed(float speed){
    this->movementSpeed = speed;
}

// Returns the z direction 
Vect3d W3Camera::getZDir(){
    return  {this->cameraMatrix.m[2][0],this->cameraMatrix.m[2][1],this->cameraMatrix.m[2][2],1};
}

// Move Forward 
void W3Camera::moveFoward(float distance){
    Vect3d newPos = this->getTranslation() + (this->getZDir() *this->movementSpeed);
    this->setTranslation(newPos);
}

// Move Forward 
void W3Camera::moveBack(){
    Vect3d newPos = this->getTranslation() + (this->getZDir() * -this->movementSpeed);
    this->setTranslation(newPos);
}


// Rotates camera
void W3Camera::rotateY(float angle){
    // Rotation mat
    Matrix4x4 rot = getMatrixRotationY(angle);

    // Rotation vector 
    Vect3d rotY = {rot.m[1][0],rot.m[1][1],rot.m[1][2]};
    // translation
    Vect3d originalTranslation = this->getTranslation();
    // Rotate 
    this->cameraMatrix = MatrixMultiplyMatrix(this->cameraMatrix,rot);
    // Restore transalation
    this->setTranslation(originalTranslation);
}

// Move up 
void W3Camera::moveUp(float distance){
    Vect3d camTrans = this->getTranslation();
    camTrans.y += distance;
    this->setTranslation(camTrans);
}


// Move left and Right
void W3Camera::moveLeft(float distance){
    Vect3d newPos = this->getTranslation() + (this->getXDir() * distance);
    this->setTranslation(newPos);
}

void W3Camera::moveRight(float distance){
    Vect3d newPos = this->getTranslation() + (this->getXDir() * -distance);
    this->setTranslation(newPos);
}