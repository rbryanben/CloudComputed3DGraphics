// Checks if a value is in a given range
bool inRange(float value,int start_value,int end_value){
    if (value >= start_value && value <= end_value)
        return true;
    return false;
}

int main(){
    bool res = inRange(2.1f,2,4);
}