import java.util.concurrent.Semaphore;
import java.util.ArrayList;
import static java.lang.Thread.sleep;

/*Used for message passing between ferry and car threads*/
public class MessagePassing {
	private int spaceOnFerry;
    private final Semaphore ferrySpaceForIsland;
    private final Semaphore ferrySpaceForMainland;
    private ArrayList<Car> carsWaitingForIsland = new ArrayList<Car>();
    private ArrayList<Car> carsWaitingForMainland = new ArrayList<Car>();
    
    /*Constructor for MessagePassing: sets ferry capacity and semaphores for island and mainland*/
    public MessagePassing(int m){
    	spaceOnFerry = m;
    	ferrySpaceForIsland  = new Semaphore(spaceOnFerry, true);
    	ferrySpaceForMainland = new Semaphore(spaceOnFerry, true);
    }
    /*Acquire semaphore, add car to queue to go to Island, and sleep*/
	public void requestFerrySpaceForIsland(Car car) throws InterruptedException {
		ferrySpaceForIsland.acquire();
		System.out.println("Car number " + car.getCarNumber() + " added to wait queue for Island.");
		carsWaitingForIsland.add(car);
		sleep(3000);
	}
	/*Acquire semaphore, add car to queue to go to Mainland, and sleep*/
	public void requestFerrySpaceForMainland(Car car) throws InterruptedException {
		ferrySpaceForMainland.acquire();
		System.out.println("Car number " + car.getCarNumber() + " added to wait queue for Mainland.");
		carsWaitingForMainland.add(car);
		sleep(3000);
	}
	/*Release semaphore for Island*/
	public void releaseFerrySpaceForIsland(){
		ferrySpaceForIsland.release();
	}
	/*Release semaphore for Mainland*/
	public void releaseFerrySpaceForMainland(){
		ferrySpaceForMainland.release();
	}
	/*Remove cars that already reached Island from queue*/
	public void removeFromIslandWaitlist(int n){
		for(int i = 0; i < n && carsWaitingForIsland.size() > 0; i++){
			carsWaitingForIsland.remove(0);
		}
	}
	/*Remove cars that already reached Mainland from queue*/
	public void removeFromMainlandWaitlist(int n){
		for(int i = 0; i < n && carsWaitingForMainland.size() > 0; i++){
			carsWaitingForMainland.remove(0);
		}
	}
	/*Return queue for cars waiting to go to Island*/
	public ArrayList<Car> getCarsWaitingForIsland(){
		return carsWaitingForIsland;
	}
	/*Return queue for cars waiting to go to Mainland*/
	public ArrayList<Car> getCarWaitingForMainland(){
		return carsWaitingForMainland;
	}
}
