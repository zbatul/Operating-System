import java.util.Random;

/* Multiple cars execute as threads */
public class Car implements Runnable {
	private int carNumber;
	private int currentCarLocation;// 0 at mainland, 1 at island
	MessagePassing msg;
	
	/*Constructor for car: sets car identifier, current location of car and MessagePassing object*/
	public Car(int carNumber, MessagePassing m) {
		this.carNumber = carNumber;
		currentCarLocation = 0;
		msg = m;
	}
	/*Runs a car thread 10 times; when it is on island it requests for mainland and vice versa.*/
	@Override
	public void run() {
		try {
			for(int i = 0; i < 10; i++){
				Random random = new Random();
			    int randomTime =  random.nextInt(1000 - 100) + 100;
				if(currentCarLocation == 0){
					System.out.println("Zzzzzz...Car number " + getCarNumber() + " is sleeping at Mainland for " + randomTime + " milliseconds");
					Thread.sleep(randomTime);
					requestFerrytoIsland();
				}
				else{
					System.out.println("Zzzzzz...Car number " + getCarNumber() + " is sleeping at Island for " + randomTime + " milliseconds");
					Thread.sleep(randomTime);
					requestFerrytoMainland();
				}
			}
		} 
		catch (Exception e) {
			e.printStackTrace();
		}
	}
	/*Passes message to request ferry to go to Island : acquire semaphore*/
	public void requestFerrytoIsland() throws Exception {
		System.out.println("Car number " + getCarNumber() + " requested ferry to Island...");
		msg.requestFerrySpaceForIsland(this);
	}
	/*Passes message to request ferry to go to Mainland : acquire semaphore*/
	public void requestFerrytoMainland() throws Exception {
		System.out.println("Car number " + getCarNumber() + " requested ferry to Mainland...");
		msg.requestFerrySpaceForMainland(this);
	}
	/*Passes message to release space on ferry to go to Island : release semaphore*/
	public void releaseFerrySpaceForIsland(){
		msg.releaseFerrySpaceForIsland();
		currentCarLocation = 0;
	}
	/*Passes message to release space on ferry to go to Mainland : release semaphore*/
	public void releaseFerrySpaceForMainland(){
		msg.releaseFerrySpaceForMainland();
		currentCarLocation = 1;
	}
	/*Returns car identifier*/
	public synchronized int getCarNumber() {
		return carNumber;
	}
}
