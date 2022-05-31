import java.util.ArrayList;

/* Ferry is a shared resource by all car threads */
public class Ferry implements Runnable {
	private int capacity;
	private int filled;
	private int currentFerryLocation; // 0 at mainland, 1 at island
	IslandParkingLot islandp;
	MessagePassing msp;
	private ArrayList<Car> carsOnFerry = new ArrayList<>();
	
	/*Constructor for Ferry: sets capacity of ferry, filled amount, current location of ferry, 
	 * and initializes MessagePassing object and IslandParkingLot object*/
	public Ferry(int M, MessagePassing msg, IslandParkingLot island){
		capacity = M;
		filled = 0;
		currentFerryLocation = 0;
		islandp = island;
		msp = msg;
	}
	/**/
	@Override
	public void run() {
		System.out.println("\nFerry service is starting from Mainland\n\n");
		for(int i = 0; i < 100; i++){
			try{
				Thread.sleep(1000);
				int requestForIsland = getNumberOfIslandRequests();
				int requestForMainland = getNumberOfMainLandRequests();
				if(currentFerryLocation == 0 && requestForIsland > 0 && islandp.getEmptyParking() > 0){
					getMainlandCars(msp.getCarsWaitingForIsland());
					goToIsland();
					dropCarsToIsland();
				}
				else if(currentFerryLocation == 1 && requestForMainland > 0){
					getIslandCars(msp.getCarWaitingForMainland());
					goToMainland();
					dropCarsToMainland();
				}
				else if(currentFerryLocation == 0 && requestForMainland > 0){
					goToIsland();
				}
				else if(currentFerryLocation == 1 && requestForIsland > 0 && islandp.getEmptyParking() > 0){
					goToMainland();
				}
			}
			catch (Exception e) {
				e.printStackTrace();
			}
		}
		System.out.println("Ferry service is stopped now. Ignoring further requests.\n\n");
	}
	/*Get number of cars in queue for going to Island*/
	public synchronized int getNumberOfIslandRequests() {
		ArrayList<Car> cars = msp.getCarsWaitingForIsland();
		return cars==null? 0: cars.size();
	}
	/*Get number of cars in queue for going to Mainland*/
	public synchronized int getNumberOfMainLandRequests() {
		ArrayList<Car> cars = msp.getCarWaitingForMainland();
		return cars==null? 0: cars.size();
	}
	/*Get the appropriate number of cars from Mainland to go to Island and remove them from waiting queue*/
	private synchronized void getMainlandCars(ArrayList<Car> cars) {
		int islandEmptySpace = islandp.getEmptyParking();
		int ferryEmptySpace = getEmptyParking();
		System.out.println("Getting mainland cars:");
		for(int i = 0; i < (islandEmptySpace>ferryEmptySpace? ferryEmptySpace:islandEmptySpace) && i < cars.size(); i++){
			carsOnFerry.add(cars.get(i));
			System.out.println("Car number " + carsOnFerry.get(i).getCarNumber() + " leaving Mainland...");
		}
		msp.removeFromIslandWaitlist(carsOnFerry.size());
	}
	/*Sets current ferry location to 1, i.e., Island*/
	private synchronized void goToIsland() {
		System.out.println("*****Ferry is going to Island*****");
		currentFerryLocation = 1;
	}
	/*Removes cars 1 by 1 from ferry, releases the semaphore and updates empty parking space at island.*/
	private synchronized void dropCarsToIsland() {
		Car c;
		for(int i = 0; i < carsOnFerry.size(); i++){
			c = carsOnFerry.get(i);
			System.out.println("Car number " + c.getCarNumber() + " landing on Island...");
			c.releaseFerrySpaceForMainland();
			filled -= 1;
		}
		islandp.setEmptyParking(-carsOnFerry.size());
		System.out.println("Island has " + islandp.getEmptyParking() + " empty parking");
		carsOnFerry.clear();
	}
	/*Get the appropriate number of cars from Island to go to Mainland and remove them from waiting queue*/
	private synchronized void getIslandCars(ArrayList<Car> cars) {
		System.out.println("Getting Island cars:");
		for(int i = 0; i < getEmptyParking() && i < cars.size(); i++){
			carsOnFerry.add(cars.get(i));
			System.out.println("Car number " + carsOnFerry.get(i).getCarNumber() + " leaving Island...");
			filled += 1;
		}
		msp.removeFromMainlandWaitlist(carsOnFerry.size());
	}
	/*Sets current ferry location to 0, i.e., Mainland*/
	private synchronized void goToMainland() {
		System.out.println("*****Ferry is going to Mainland*****");
		currentFerryLocation = 0;
	}
	/*Removes cars 1 by 1 from ferry, releases the semaphore and updates empty parking space at island.*/
	private synchronized void dropCarsToMainland() {
		Car c;
		for(int i = 0; i < carsOnFerry.size(); i++){
			c = carsOnFerry.get(i);
			System.out.println("Car number " + c.getCarNumber() + " landing on Mainland...");
			c.releaseFerrySpaceForIsland();
			filled -= 1;
		}
		islandp.setEmptyParking(carsOnFerry.size());
		System.out.println("Island has " + islandp.getEmptyParking() + " empty parking");
		carsOnFerry.clear();
	}
	/*Getter for empty parking space on ferry*/
	private synchronized int getEmptyParking(){
		return capacity - filled;
	}
}
