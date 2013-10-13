//  AwfulFetchedResultsControllerDataSource.h
//
//  Copyright 2013 Awful Contributors. CC BY-NC-SA 3.0 US https://github.com/Awful/Awful.app

#import <UIKit/UIKit.h>
@protocol AwfulFetchedResultsControllerDataSourceDelegate;

/**
 * An AwfulFetchedResultsControllerDataSource acts as a generic UITableViewDataSource on behalf of an NSFetchedResultsController.
 */
@interface AwfulFetchedResultsControllerDataSource : NSObject <UITableViewDataSource>

/**
 * Returns an initialized AwfulFetchedResultsControllerDataSource. This is the designated initializer.
 *
 * @param tableView       The table view whose data the AwfulFetchedResultsControllerDataSource object provides.
 * @param reuseIdentifier A UITableViewCell reuse identifier for dequeueing cells from the table view.
 */
- (id)initWithTableView:(UITableView *)tableView reuseIdentifier:(NSString *)reuseIdentifier;

/**
 * The table view whose data is provided by the AwfulFetchedResultsControllerDataSource.
 */
@property (readonly, weak, nonatomic) UITableView *tableView;

/**
 * The UITableViewCell reuse identifier for dequeueing cells from the table view.
 */
@property (readonly, copy, nonatomic) NSString *reuseIdentifier;

/**
 * The fetched results controller that provides the data.
 */
@property (strong, nonatomic) NSFetchedResultsController *fetchedResultsController;

/**
 * The delegate.
 */
@property (weak, nonatomic) id <AwfulFetchedResultsControllerDataSourceDelegate> delegate;

@end

@protocol AwfulFetchedResultsControllerDataSourceDelegate <NSObject>

/**
 * Asks the delegate to configure a UITableViewCell to represent a model object.
 *
 * Both parameters are typed as `id` so that compliant method prototypes can specify a subclass and avoid explicit casting.
 *
 * @param cell   The cell to configure.
 * @param object The model object to use for configuration.
 */
- (void)configureCell:(id)cell withObject:(id)object;

@end
