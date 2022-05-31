public class IslandParkingLot {
	private int capacity;
	private int filled;
	/*Constructor for IslandParkingLot: sets capacity and how much filled parameters*/
	public IslandParkingLot(int N){
		capacity = N;
		filled = 0;
	}
	/*Getter for empty parking space at island*/
	public synchronized int getEmptyParking(){
		return capacity - filled;
	}
	/*Setter for empty parking space at island*/
	public synchronized void setEmptyParking(int e){
		filled -= e;
	}
}
